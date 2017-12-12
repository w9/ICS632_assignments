% Final Project: Parallelizing the t-SNE algorithm
% Xun Zhu

Abstract
========


Introduction
============

t-Distributed Stochastic Neighbor Embedding (t-SNE) is a dimensional reduction
algorithm that has been widely used in the bioinformatics field in recent years
[1-5]. It's unique ability to help visualize the complex spatial relationships
between high-dimensional points (vectors) makes it suitable for the
state-of-the-art next generation sequencing technologies. t-SNE has also been
popular in other fields that have high-dimensional data, the most well-known
example being the "deep-learning" research community [6-8].

The key idea of t-SNE is Stochastic Neighbor Embedding (SNE). For each pair of
points $x_i$ and $x_j$, the *similarity* between $x_i$ and $x_j$ is defined as
the conditional probability at $x_j$, if we set a Gaussian distribution centered
at $x_i$. This conditional probability, denoted as $p_{j|i}$, is given by

  $$p_{j|i} = \frac{\exp (-\text{dist}^2(x_i, x_j) / 2 \sigma^2)}{\exp (\sum_{k \ne i}-\text{dist}^2(x_i, x_j) / 2 \sigma^2)}$$


References
==========

[1] Yamaji, Masashi, et al. "DND1 maintains germline stem cells via recruitment of the CCR4–NOT complex to target mRNAs." Nature 543.7646 (2017): 568-572.
[2] Esteva, Andre, et al. "Dermatologist-level classification of skin cancer with deep neural networks." Nature 542.7639 (2017): 115-118.
[3] Habib, Naomi, et al. "Massively parallel single-nucleus RNA-seq with DroNc-seq." nature methods 14.10 (2017): 955-958.
[4] Enge, Martin, et al. "Single-Cell Analysis of Human Pancreas Reveals Transcriptional Signatures of Aging and Somatic Mutation Patterns." Cell 171.2 (2017): 321-330.
[5] Smith, Austin Gerard, et al. "Epigenetic resetting of human pluripotency." (2017).
[6] Inglese, Paolo, et al. "Deep learning and 3D-DESI imaging reveal the hidden metabolic heterogeneity of cancer." Chemical Science 8.5 (2017): 3500-3511.
[7] White, Martin, et al. "Sorting and Transforming Program Repair Ingredients via Deep Learning Code Similarities." arXiv preprint arXiv:1707.04742 (2017).
[8] Kim, Jaebok, et al. "Towards Speech Emotion Recognition" in the wild" using Aggregated Corpora and Deep Multi-Task Learning." arXiv preprint arXiv:1708.03920 (2017).



