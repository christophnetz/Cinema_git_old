# Cinema

Cinema is an individual-based model that simulates the coevolution of movement strategies between predators and prey in a spatially explicit environment. 


## System requirements

The program uses Windows C++ libraries and therefore depends on a Windows platform. 
Further,OpenGL 4.6 or higher is required.


## Versions

Users interested in the reproduction of our published data should turn to the master branch of this repository. 
A simplified model version is available under the demo branch. Users who just wish to try different parameters can 
download a zip-folder with the executable, premade batch scripts and config files. Parameters can of course also be changed 
manually within the config files.


## Parameters

### G
Parameter G sets the number of generations that the program is going to run. Each generation consists of T time steps.

### T
Parameter T sets the number of time steps per generation, and therefore also the number of movements an individual makes 
before reproduction occurs. This parameter can be of interest in the creation of ecological patterns: Low T (e.g. 2)
produces spatial patterns that are driven primarily by differential survival and offspring dispersal, while large T (e.g. 500)
create spatial patterns primarily driven by the movement decisions of individuals during their lifetimes. 
We used T = 100 as the default setting.

### N
The parameters pred.N and prey.N sets the population size for predators (fixed) and prey (fixed at the beginning of a generation, then reduced by predation),
and the ratio between them. We used prey.N = 10,000 as default and either pred.N = 10,000 or pred.N = 1,000 to vary between
a predator-prey ratio of 1:1 and 1:10.

### L
The parameter L sets the sensory and movement range diameter for predators and prey and can take the values of {3, 5, 7, 33}. 
A value of L = 3 describes a movement range equivalent to the Moore Neighborhood. Default parameters were prey.L = 3 and pred.L = 5. 
It is useful to set the predator movement range larger than the prey's, because this enables predators to catch up 
with prey and complicates the prey's task from simply moving away from predators, to being unpredictable to them. 

### ann
The parameter ann sets the artificial neural network to be used as a decision-making mechanism by the populations.
There are five types of predefined neural network topologies: DumbAnn, SimpleAnn_identitynf, SimpleAnn, SimpleAnnFB and
SmartAnn. DumbAnn implements random movements regardless of information inputs. SimpleAnn_identitynf has three weighing factors,
one per input, and an identity node function. This was the node function used in all our simulations shown in the publication.
SimpleAnn equally has three weighing factors, but applies a rectifier node function. The only difference to SimpleAnn_identitynf
is that individuals can move randomly, if all cells are evaluated negatively (-> 0). SimpleAnnFB is equivalent to SimpleAnn,
except that it contains two recursive weights, enabling individuals to keep track of past inputs. SmartAnn does not have recursive
weights, but implements an intermediate hidden layer of three nodes, resulting in a total of 16 weighing factors. Due to the 
intermediate layer, more complex input-output relations can be realized. 

### Sprout_radius
Sprout_radius sets the dispersal range, in which offspring are distributed. Sprout_radius of 1 corresponds again to 
the Moore Neighborhood. We used Sprout_radius values of 1 and 10 as our default parameters. For global offspring dispersal,
Sprout_radius can be set to 256 (or landscape.dim/2).

### mutation_prob
The mutation_prob parameter sets the probability, with which a network weight mutates during reproduction. 
Mutations are applied to offspring and effect sizes are drawn from a Cauchy distribution. 
By default, mutation_prob =  0.001.

### mutationstep
If a weight mutates, the new weight value is given by the old value plus a mutation effect 
that is drawn from a Cauchy distribution with location 0 and scale equal to the mutationstep parameter.
By default, mutationstep = 0.001.

### noise_sigma
The information inputs, based on which individuals made movement decisions, were subjected to some perceptual noise that 
was implemented as follows: Inputs were multiplied by a random value drawn from a uniform distribution U(1.0 - noise_sigma, 1.0 + noise_sigma).
By default, noise_sigma = 0.1. An interesting question of future research could be, how perceptual noise evolves during predator-prey coevolution and
in the presence of an energetic trade-off.

### input_mask
The input_mask parameter can be used to turn off any of the three information inputs {prey, pred, grass}.
For example, figure 4 in our manuscript shows a simulation, where prey.input_mask was {1,1,1} and pred.input_mask was {0,1,0}.
By default, however, all information inputs are turned on (input_mask = {1,1,1}). 

### grass_growth
The grass_growth parameter controls the amount by which grass increases in each time step, up until a maximum value of 1 
is reached. By default, grass growth = 0.01

### predation_chance
The predation_chance parameter determines, how likely an encounter between a predator and a prey on the same cell 
will result in a successful predation event, during which the prey is killed and consumed by the predator.
By default, predation_chance = 0.5

### dim
The dim parameter controls the edge length of the landscape, which is a square-shaped grid with wrapped boundaries (thus a torus).
The number of cells is thus dim squared, and the default parameter is dim = 512.



## Authors
Christoph Netz, Hanno Hildenbrandt, Franz J. Weissing

## License
[MIT](https://choosealicense.com/licenses/mit/)