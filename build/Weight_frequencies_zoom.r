

args = commandArgs(trailingOnly=TRUE)

source(paste0(args, "\\sourceMe.r"))
config$dir = args # set dir explicit

#install.packages("lattice") 
#library(lattice)
require(lattice)
require(cowplot)
require(gridExtra)
require(png)


gens = config$G 
#fitmeans_P =  SMRY$pred[,1]
#fitmeans_H = SMRY$pred[,1]

# div_P = vector() analog to SMRY$pred[,4]?
# div_H = vector()

minwv = -1.01;     # minimal (weight) value
maxwv = 1.01;     # maximal (weight) value
steps = 101;  # num. of bins across range
stepsize = (maxwv - minwv)/steps  # bin range size
cc = colorRampPalette(c("white", "red", "blue"), bias = 2)(100)

colkey = draw.colorkey(key = list(col = cc, at=seq(0, 1, 0.1), height = 0.75), draw = T, vp=NULL)


#Frequency matrix:
#Predators
mtrxwP1 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwP2 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwP3 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwP4 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
#Herbivores
mtrxwH1 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwH2 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwH3 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
mtrxwH4 <- matrix(nrow = steps, ncol = gens, dimnames=list(seq(minwv+stepsize, maxwv, stepsize)))  # Frequency matrix
#i=1; j=1


# start.time <- Sys.time()
for (i in 1:gens){
  G = generation((i-1))
  
  
  #Weights
  mtrxwP1[,i] = table(cut(tanh(G$pred$ann[,1] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$pred$ann[,1])
  mtrxwP2[,i] = table(cut(tanh(G$pred$ann[,2] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$pred$ann[,2])
  mtrxwP3[,i] = table(cut(tanh(G$pred$ann[,3] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$pred$ann[,3])
  mtrxwP4[,i] = table(cut(tanh(G$pred$ann[,4] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$pred$ann[,4])
  
  
  mtrxwH1[,i] = table(cut(tanh(G$prey$ann[,1] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$prey$ann[,1])
  mtrxwH2[,i] = table(cut(tanh(G$prey$ann[,2] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$prey$ann[,2])
  mtrxwH3[,i] = table(cut(tanh(G$prey$ann[,3] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$prey$ann[,3])
  mtrxwH4[,i] = table(cut(tanh(G$prey$ann[,4] * 100), seq(minwv, maxwv, stepsize), right=T))/length(G$prey$ann[,4])
  
  
  
}
# end.time <- Sys.time()
# time.taken <- end.time - start.time
# time.taken

#SMRY = summary()



# Predators


P1P <- levelplot(mtrxwP1, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "generation", main = "node bias", colorkey = FALSE)

if(config$pred.input_mask[1] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 

P2P <- levelplot(mtrxwP2, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("prey density", onoff), colorkey = FALSE)

if(config$pred.input_mask[2] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 

P3P <- levelplot(mtrxwP3, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("predator density", onoff), colorkey = FALSE)

if(config$pred.input_mask[3] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 

P4P <- levelplot(mtrxwP4, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("grass", onoff), colorkey = FALSE)


#Herbivores



P1H <- levelplot(mtrxwH1, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "generation", main = "node bias", colorkey = FALSE)


if(config$prey.input_mask[1] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 


P2H <- levelplot(mtrxwH2, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("prey density", onoff), colorkey = FALSE)

if(config$prey.input_mask[2] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 

P3H <- levelplot(mtrxwH3, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("predator density", onoff), colorkey = FALSE)

if(config$prey.input_mask[3] == 0){
  onoff = ", off"
} else{
  onoff = ""
  
} 

P4H <- levelplot(mtrxwH4, aspect="fill", col.regions = cc,
                 panel = function(...){
                   panel.levelplot(...)
                   panel.abline(v = 51, lwd = 0.35)
                 }, 
                 scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
                 xlab = "weight value", ylab = "", main = paste0("grass", onoff), colorkey = FALSE)

Pred = grid.arrange(P1P, P4P, P2P, P3P, colkey, ncol=5, nrow=1, widths = c(1,1,1,1,0.5))
Herb = grid.arrange(P1H, P4H, P2H, P3H, colkey, ncol=5, nrow=1, widths = c(1,1,1,1,0.5)) 


png("trial.png", width=2000,height=1400, res = 144)
plot_grid(Herb, Pred,  labels = c('Prey', 'Pred'),label_size = 20, hjust = 0, ncol = 1, align = 'v')

dev.off()



plotPNG = readPNG("trial.png")
pdf(paste0(args, paste0("\\", config$outdir, "_weights_2.pdf")), width = 10, height = 7)
par(mai=c(0,0,0,0))
plot(c(0,1),c(0,1),type="n")
rasterImage(plotPNG,0,0,1,1)
dev.off()

file.remove("trial.png")

###########################################
# P3H <- levelplot(mtrxwH3[,1:5000], aspect="fill", col.regions = cc,
#                  panel = function(...){
#                    panel.levelplot(...)
#                    panel.abline(v = 51, lwd = 0.35)
#                  }, 
#                  scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
#                  xlab = "weight value", ylab = "generation", main = paste0("", onoff), colorkey = FALSE)
# 
# if(config$prey.input_mask[3] == 0){
#   onoff = ", off"
# } else{
#   onoff = ""
#   
# } 
# 
# P3P <- levelplot(mtrxwP3[,1:5000], aspect="fill", col.regions = cc,
#                  panel = function(...){
#                    panel.levelplot(...)
#                    panel.abline(v = 51, lwd = 0.35)
#                  }, 
#                  scales = list(x=list(at=c(1,(steps/2 + 0.5),steps),labels=c("-1.0", "0.0", "1.0"))), 
#                  xlab = "weight value", ylab = "", main = paste0("", onoff), colorkey = FALSE)
# 
# if(config$pred.input_mask[3] == 0){
#   onoff = ", off"
# } else{
#   onoff = ""
#   
# } 
# 
# 
# pdf("polymorphism_weights-27-2.pdf", width = 10, height = 8)
# grid.arrange(P3H, P3P, ncol=2, nrow=1, widths = c(1,1)) 
# dev.off()
