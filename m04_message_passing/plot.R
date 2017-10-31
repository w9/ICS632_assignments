library(tidyverse)


algos <- c('naive', 'ring')

tb_list <- list()
for (algo in algos) {
  tb_list[[algo]] <-
    read_csv(sprintf('log_%s.csv', algo), col_names=c('rank', 'begin', 'end', 'event'), col_types='cddc')
}
tb <- tb_list %>% enframe('algo', 'data') %>% unnest %>%
    mutate(rank=stringr::str_pad(rank, 2, 'left', '0')) %>%
    mutate(color=sample(c(letters, LETTERS), n()))

print(tb)

p <-
  ggplot(tb) +
  facet_wrap(~ algo, ncol=1) +
  geom_segment(aes(x=rank, y=begin, xend=rank, yend=end, color=color), size=3) +
  geom_text(aes(x=rank, y=begin, label=event), vjust=-1, hjust=0) +
  labs(x='Rank', y='Time') +
  coord_flip() +
  theme(legend.position='none')

ggsave(sprintf('timeline.pdf', algo), p, width=10, height=7)
