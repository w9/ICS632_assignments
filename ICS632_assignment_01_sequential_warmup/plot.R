library(tidyverse)

args <- commandArgs(trailingOnly=T)

if (length(args) != 1) {
  message(sprintf('The number of arguments is not correct: 1 expected, %d received.', length(args)))
}

ggdat <- read_csv(args[1], comment='#') %>%
  filter(algo=='tiled') %>%
  mutate(bs=factor(bs)) %>%
  gather(measure_type, measure_value, wall_time, l1_load_misses, llc_load_misses)

p <- ggplot(ggdat) +
  geom_boxplot(aes(bs, measure_value)) +
  facet_wrap(~ measure_type, ncol=1, scales='free_y') +
  theme(axis.text.x=element_text(angle=30, hjust=1))

ggsave('boxplot.png', p, width=60, height=21, limitsize=F)
