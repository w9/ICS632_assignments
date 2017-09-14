library(tidyverse)
library(ggthemes)

args <- commandArgs(trailingOnly=T)

if (length(args) != 1) {
  message(sprintf('The number of arguments is not correct: 1 expected, %d received.', length(args)))
}

tb <- read_csv(args[1], comment='#')

ggdat <- tb %>%
  filter(rep > 0) %>%
  filter(algo == 'tiled') %>%
  filter(bs %in% (round(2^seq(0, 20, 0.5)))) %>%
  ## mutate(bs=factor(bs)) %>%
  gather(measure_type, measure_value, wall_time, l1_load_misses, llc_load_misses) %>%
  group_by(bs, measure_type) %>%
  summarize(mean_value = mean(measure_value),
            max_value = max(measure_value),
            min_value = min(measure_value),
            sd = sd(measure_value))

print(ggdat)
ggdat %>% filter(measure_type == 'l1_load_misses') %>% arrange(mean_value) %>% print

benchmarks_tb <- tb %>%
  filter(algo != 'tiled') %>%
  gather(measure_type, measure_value, wall_time, l1_load_misses, llc_load_misses) %>%
  group_by(algo, measure_type) %>%
  summarize(mean_value = mean(measure_value))

print(benchmarks_tb)

algo_offset = c(
  ij = 20,
  ji = 50,
  tiled = 70
)

algo_tr = c(
  ij = 'i-j',
  ji = 'j-i',
  tiled = 'Tiled'
)

measure_type_tr = c(
  wall_time = 'Wall-clock time (seconds)',
  l1_load_misses = 'L1 load misses',
  llc_load_misses = 'LLC load misses'
)

print(benchmarks_tb)

p <- ggplot(ggdat) +
  ## geom_boxplot(aes(bs, measure_value)) +
  geom_hline(aes(yintercept=mean_value), benchmarks_tb, color='pink') +
  geom_label(aes(algo_offset[algo], mean_value, label=algo_tr[algo]), benchmarks_tb, color='white', fill='white', family='Times') +
  geom_text(aes(algo_offset[algo], mean_value, label=algo_tr[algo]), benchmarks_tb, color='pink', family='Times') +
  geom_ribbon(aes(bs, ymin=min_value, ymax=max_value), alpha=0.4) +
  geom_line(aes(bs, mean_value)) +
  facet_wrap(~ measure_type, ncol=1, scales='free_y', labeller=labeller(measure_type=measure_type_tr)) +
  theme_tufte() +
  scale_x_log10() +
  ## theme(panel.grid.major.y=element_line(color='gray80', linetype='dotted', size=0.2)) +
  labs(x='Block size', y='Mean value', title='GCC')
  ## theme(axis.text.x=element_text(angle=30, hjust=1))

ggsave('boxplot.png', p, width=7, height=5, limitsize=F)
