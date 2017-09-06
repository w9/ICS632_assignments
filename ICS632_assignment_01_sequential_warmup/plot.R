library(tidyverse)

ggdat <- read_csv('./results.csv') %>%
  filter(algo=='tiled') %>%
  mutate(bs=factor(bs)) %>%
  gather(measure_type, measure_value, wall_time, l1_load_misses, lcc_load_misses)

p <- ggplot(ggdat) +
  geom_boxplot(aes(bs, measure_value)) +
  facet_wrap(~ measure_type, ncol=1, scales='free_y') +
  theme(axis.text.x=element_text(angle=30, hjust=1))

ggsave('boxplot.png', p, width=60, height=21, limitsize=F)
