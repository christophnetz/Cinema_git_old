#include <iostream>
#include <filesystem>
#include "simulation.h"
#include "game_watches.hpp"
#include "cmd_line.h"


namespace filesystem = std::filesystem;


namespace cine2 {


  Simulation::Simulation(const Param& param)
    : g_(-1), t_(-1),
    param_(param)
  {
    using Layers = Landscape::Layers;

    prey_.pop = std::vector<Individual>(param.prey.N);
    prey_.tmp_pop = std::vector<Individual>(param.prey.N);
    prey_.ann = make_any_ann(param.prey.L, param.prey.N, param.prey.ann.c_str());
    prey_.fitness = std::vector<float>(param.prey.N, 0.f);
    prey_.tmp_ann = make_any_ann(param.prey.L, param.prey.N, param.prey.ann.c_str());

    pred_.pop = std::vector<Individual>(param.pred.N);
    pred_.tmp_pop = std::vector<Individual>(param.pred.N);
    pred_.ann = make_any_ann(param.pred.L, param.pred.N, param.pred.ann.c_str());
    pred_.fitness = std::vector<float>(param.pred.N, 0.f);
    pred_.tmp_ann = make_any_ann(param.pred.L, param.pred.N, param.pred.ann.c_str());

    // initialize rdist
    assess_fitness();

    // initialize landscape
    init_layer();
    if (landscape_.dim() < 32) throw std::runtime_error("Landscape too small");

    // full grass cover
    for (auto& g : landscape_[Layers::grass]) g = param.landscape.max_grass_cover;

    // initial positions
    auto coorDist = std::uniform_int_distribution<short>(0, short(landscape_.dim() - 1));
    for (auto& p : prey_.pop) { p.pos.x = coorDist(rnd::reng); p.pos.y = coorDist(rnd::reng); }
    for (auto& p : pred_.pop) { p.pos.x = coorDist(rnd::reng); p.pos.y = coorDist(rnd::reng); }

    // initial occupancies and observable densities
    landscape_.update_occupancy(Layers::prey_count, Layers::prey, prey_.pop.cbegin(), prey_.pop.cend(), param_.landscape.prey_kernel);
    landscape_.update_occupancy(Layers::pred_count, Layers::pred, pred_.pop.cbegin(), pred_.pop.cend(), param_.landscape.pred_kernel);

    // optional: initialization from former runs
    if (!param_.init_prey_ann.empty()) {
      init_anns_from_archive(prey_, archive::iarch(param_.init_prey_ann));
    }
    if (!param_.init_pred_ann.empty()) {
      init_anns_from_archive(pred_, archive::iarch(param_.init_pred_ann));
    }
  }


  void Simulation::init_anns_from_archive(Population& Pop, archive::iarch& ia)
  {
    auto cm = ia.extract(param_.initG >= 0 ? std::min(param_.initG, param_.G - 1) : param_.G - 1);
    if (cm.un != Pop.ann->N()) throw cmd::parse_error("Number of ANNs doesn't match");
    if (cm.usize != Pop.ann->type_size()) throw cmd::parse_error("ANN state size doesn't match");
    auto dst = Pop.ann->data();
    uncompress(dst, cm, Pop.ann->stride() * sizeof(float));
  }


  namespace detail {


    template <typename FITNESSFUN>
    void assess_fitness(Population& population,
      const Param::ind_param& iparam,
      FITNESSFUN fitness_fun)
    {
      const auto& pop = population.pop;
      const auto& ann = population.ann;
      auto& fitness = population.fitness;
      const int N = static_cast<int>(pop.size());
#     pragma omp parallel for schedule(static)
      for (int i = 0; i < N; ++i) {
        fitness[i] = fitness_fun(pop[i]);
      }
      population.rdist.mutate(fitness.cbegin(), fitness.cend());
    }


    void create_new_generation(const Landscape& landscape,
      Population& population,
      const Param::ind_param& iparam,
      bool fixed)
    {
      const auto& pop = population.pop;
      const auto& ann = *population.ann;
      const int N = static_cast<int>(pop.size());
#     pragma omp parallel 
      {
        auto& tmp_pop = population.tmp_pop;
        auto& tmp_ann = *population.tmp_ann;
        const auto& rdist = population.rdist;
        const auto coorDist = rndutils::uniform_signed_distribution<short>(-iparam.sprout_radius, iparam.sprout_radius);
#       pragma omp for schedule(static)
        for (int i = 0; i < N; ++i) {
          const int ancestor = rdist(rnd::reng);
          auto newPos = pop[ancestor].pos + Coordinate{ coorDist(rnd::reng), coorDist(rnd::reng) };
          tmp_pop[i].sprout(landscape.wrap(newPos), ancestor);
          tmp_ann.assign(ann, ancestor, i);   // copy ann
        }
      }
      population.tmp_ann->mutate(iparam, fixed);

      using std::swap;
      swap(population.pop, population.tmp_pop);
      swap(population.ann, population.tmp_ann);
    }

  }


#define simulation_observer_notify(msg) \
  if ((observer ? !observer->notify(this, msg) : true)) return false


  bool Simulation::run(Observer* observer)
  {
    // burn-in
    simulation_observer_notify(INITIALIZED);
    const int Gb = param_.Gburnin;
    for (int gb = 0; gb < Gb; ++gb) {
      simulation_observer_notify(NEW_GENERATION);
      const int Tb = param_.T;
      for (int tb = 0; tb < Tb; ++tb) {
        simulate_timestep();
        simulation_observer_notify(WATCHDOG);   // app alive?
      }

      //assess_fitness(); //CN: fix?
      // clear fitness
      prey_.fitness.assign(prey_.fitness.size(), 0.f);
      pred_.fitness.assign(pred_.fitness.size(), 0.f);
      //assess_fitness(); //CN: fix?
      create_new_generations();
    }
    const int G = param_.G;
    for (g_ = 0; g_ < G; ++g_) {
      simulation_observer_notify(NEW_GENERATION);
      const int T = fixed() ? param_.Tfix : param_.T;
      for (t_ = 0; t_ < T; ++t_) {
        simulate_timestep();
        simulation_observer_notify(POST_TIMESTEP);

      }



      assess_fitness();
      analysis_.generation(this);
      simulation_observer_notify(GENERATION);
      create_new_generations();
    }
    simulation_observer_notify(FINISHED);
    return true;
  }

#undef simulation_observer_notify


  void Simulation::simulate_timestep()
  {
    using Layers = Landscape::Layers;

    // grass growth
    const int DD = landscape_.dim() * landscape_.dim();
    float* __restrict cover = landscape_[Layers::grass].data();
    ann_assume_aligned(cover, 32);
    const float max_grass_cover = param_.landscape.max_grass_cover;
    const float grass_growth = param_.landscape.grass_growth;
#   pragma omp parallel for schedule(static)
    for (int i = 0; i < DD; ++i) {
      cover[i] = std::min(max_grass_cover, cover[i] + grass_growth);
    }

    // move
    prey_.ann->move(landscape_, prey_.pop, param_.prey);
    pred_.ann->move(landscape_, pred_.pop, param_.pred);

    // update occupancies and observable densities
#   pragma omp parallel sections
    {
#     pragma omp section
      landscape_.update_occupancy(Layers::prey_count, Layers::prey, prey_.pop.cbegin(), prey_.pop.cend(), param_.landscape.prey_kernel);
#     pragma omp section
      landscape_.update_occupancy(Layers::pred_count, Layers::pred, pred_.pop.cbegin(), pred_.pop.cend(), param_.landscape.pred_kernel);
    }

    resolve_grazing_and_attacks();
  }


  void Simulation::assess_fitness()
  {
    detail::assess_fitness(prey_, param_.prey, Param::prey_fitness);
    detail::assess_fitness(pred_, param_.pred, Param::pred_fitness);
  }


  void Simulation::create_new_generations()
  {
    detail::create_new_generation(landscape_, prey_, param_.prey, fixed());
    detail::create_new_generation(landscape_, pred_, param_.pred, fixed());
  }


  void Simulation::resolve_grazing_and_attacks()
  {
    using Layers = Landscape::Layers;
    LayerView prey_count = landscape_[Layers::prey_count];
    LayerView pred_count = landscape_[Layers::pred_count];
    LayerView grass = landscape_[Layers::grass];
    LayerView old_grass = landscape_[Layers::temp];
    old_grass.copy(grass);

    // find *attacking* predators
    attacking_pred_.clear();
    auto last_pred = pred_.pop.data() + pred_.pop.size();
    for (auto pred = pred_.pop.data(); pred != last_pred; ++pred) {
      const Coordinate pos = pred->pos;
      if (prey_count(pos)) {
        if (std::bernoulli_distribution(0.5)(rnd::reng)) {
          attacking_pred_.push_back(pred);
        }
        else {
          // non-attacking predator, remove
          --pred_count(pos);
        }
      }
    }

    // find *attacked* prey and graze on the fly
    attacked_prey_.clear();
    auto last_prey = prey_.pop.data() + prey_.pop.size();
    for (auto prey = prey_.pop.data(); prey != last_prey; ++prey) {
      if (prey->alive()) {
        const Coordinate pos = prey->pos;
        if (pred_count(pos)) {
          attacked_prey_.push_back(prey);
        }
        prey->food += old_grass(pos) / prey_count(pos);
        grass(pos) = 0.f;  //   depleted

      }
    }

    // resolve 'interactions'. This is now 
    //   O(#attacking pred * #attacked prey) 
    // instead of
    //   O(#pred * #prey)
    for (auto pred : attacking_pred_) {
      for (auto prey : attacked_prey_) {
        const Coordinate pos = prey->pos;
        if (pred->pos == pos) {
          prey->die();
          pred->food += 1.f / pred_count(pos);
        }
      }
    }
  }


  void Simulation::init_layer()
  {
    if (landscape_.dim() == 0) {
      landscape_ = Landscape(512); //replace with dim parameter
    }
  }


  class SimpleObserver : public Observer
  {
  public:
    SimpleObserver() {}
    ~SimpleObserver() override {}

    // required observer interface
    bool notify(void* userdata, long long msg) override
    {
      auto sim = reinterpret_cast<Simulation*>(userdata);
      using msg_type = Simulation::msg_type;
      switch (msg) {
      case msg_type::INITIALIZED:
        std::cout << "Simulation initialized\n";
        break;
      case msg_type::NEW_GENERATION: {
        watch_.reset();
        watch_.start();
        std::cout << "Generation: " << sim->generation() << (sim->fixed() ? "*  " : "   ");
        break;
      }
      case msg_type::GENERATION: {
        std::cout << sim->analysis().prey_summary().back().ave_fitness << "   ";
        std::cout << sim->analysis().prey_summary().back().repro_ind << "   ";
        std::cout << sim->analysis().prey_summary().back().repro_ann << "  (";
        std::cout << sim->analysis().prey_summary().back().complexity << ");   ";

        std::cout << sim->analysis().pred_summary().back().ave_fitness << "   ";
        std::cout << sim->analysis().pred_summary().back().repro_ind << "   ";
        std::cout << sim->analysis().pred_summary().back().repro_ann << "  (";
        std::cout << sim->analysis().pred_summary().back().complexity << ");   ";

        std::cout << (int)(1000 * watch_.elapsed().count()) << "ms\n";
        break;
      }
      case msg_type::FINISHED:
        std::cout << "\rSimulation finished\n";
        break;
      }
      return notify_next(userdata, msg);
    };

  private:
    game_watches::Stopwatch watch_;
  };


  std::unique_ptr<Observer> CreateSimpleObserver()
  {
    return std::unique_ptr<Observer>(new SimpleObserver());
  }

}
