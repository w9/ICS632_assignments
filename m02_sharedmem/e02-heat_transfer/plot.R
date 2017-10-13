library(tidyverse)
library(ggthemes)

args <- commandArgs(trailingOnly=T)

if (length(args) != 1) {
  message(sprintf('The number of arguments is not correct: 1 expected, %d received.', length(args)))
}

tb <- read_csv(args[1], comment='#')

algo_tr = c(
  'Sequential',
  'Naive parallelism',
  'Blocked parallelism'
)

ggdat <- tb %>%
  filter(ALGO > 0) %>%
  mutate(ALGO=algo_tr[ALGO + 1]) %>%
  group_by(ALGO, NUM_THREADS) %>%
  summarize(mean_time=mean(RUNNING_TIME),
            min_time=min(RUNNING_TIME),
            max_time=max(RUNNING_TIME)
            )

print(ggdat)

## ggdat %>% filter(measure_type == 'l1_load_misses') %>% arrange(mean_value) %>% print

benchmarks_tb <- tb %>%
  filter(ALGO == 0) %>%
  print %>%
  mutate(ALGO=algo_tr[ALGO + 1]) %>%
  group_by(ALGO, NUM_THREADS) %>%
  summarize(mean_time=mean(RUNNING_TIME),
            min_time=min(RUNNING_TIME),
            max_time=max(RUNNING_TIME)
            )

## print(benchmarks_tb)

## algo_offset = c(
##   ij = 20,
##   ji = 50,
##   tiled = 70
## )

## algo_tr = c(
##   ij = 'i-j',
##   ji = 'j-i',
##   tiled = 'Tiled'
## )

## print(benchmarks_tb)

p <- ggplot(ggdat) +
  ## geom_boxplot(aes(bs, measure_value)) +
  geom_hline(aes(yintercept=mean_time), benchmarks_tb, color='pink') +
  geom_label(aes(5, mean_time, label='Sequential'), benchmarks_tb, color='white', fill='white', family='Times') +
  geom_text(aes(5, mean_time, label='Sequential'), benchmarks_tb, color='pink', family='Times') +
  geom_ribbon(aes(NUM_THREADS, ymin=min_time, ymax=max_time, group=ALGO), alpha=0.4) +
  geom_line(aes(NUM_THREADS, mean_time, group=ALGO, linetype=ALGO)) +
  ## facet_wrap(~ ALGO, ncol=1, scales='free_y', labeller=labeller(ALGO=algo_tr)) +
  scale_linetype_discrete(name='Algorithm') +
  theme_tufte() +
  ## scale_x_log10() +
  ## theme(panel.grid.major.y=element_line(color='gray80', linetype='dotted', size=0.2)) +
  labs(x='Number of threads', y='Mean running time in seconds')
  ## theme(axis.text.x=element_text(angle=30, hjust=1))

ggsave('boxplot.png', p, width=7, height=5, limitsize=F)
