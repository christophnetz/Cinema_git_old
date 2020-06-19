args = commandArgs(trailingOnly=TRUE)

source(paste0(args, "\\sourceMe.r"))
config$dir = args # set dir explicit

#install.packages("lattice") 
#library(lattice)
require(ggplot2)
require(cowplot)
require(gridExtra)

gens = config$G

SMRY = summary()


x1 <- data.frame("Generations" = 1:length(SMRY$prey[,1]), "Performance" = SMRY$prey[,1])
x2 <- data.frame("Generations" = 1:length(SMRY$pred[,1]), "Performance" = SMRY$pred[,1])


a <- ggplot(data=x1, aes(x=Generations, y=Performance)) + labs(x="Generations", y="Prey performance") +
  geom_line(color="black")  + theme(axis.title=element_text(size=12)) 


a <- a + geom_line(data = x2, aes(x = Generations, y = (Performance)*5), color="red")  + scale_y_continuous(limits = c(10, 25), sec.axis = sec_axis(~./5, name = "Predator performance")) 
#a



pdf(paste0(args, paste0("\\", config$outdir, "_fitness3.pdf")), width = 14, height = 7)
a
dev.off()