# https://stephens999.github.io/fiveMinuteStats/intro_to_mixture_models.html
# https://sites.nicholas.duke.edu/statsreview/jmc/

# Mixture models are probability models where there are multiple probability distributions (components) one can sample from an outcome.
# For every sample from the mixture model, a random choice is made as to which probability distribution to sample from.
# Gaussian mixture models are the most common (this is where gaussian distributions represent the components)
#
# Mixture models can lead to multimodal distributions, where the distribution has multiple regions with high probability mass

# Notation:
# X_i        A sample we obtain when sampling from the distribution.
# Z_i        A latent variable. Latent variables represents which distribution (component) we draw from and it is associated with each sample X_i.
# Pi_k       Mixture weights. They represent the probability that X belongs to the kth mixture component.
# (X_i, Z_i) Denotes which component Z_i sample X_i came from.

# Definitions:
# P(A)      Marginal/Unconditional probability, the probability of an event occuring.
# P(A ∩ B)  Joint probability,  the probability of A and B occuring.
# P(A | B)  Conditional probability, the probability of A occuring given that B occured. 

# Using the law of total probability, we can determine the marginal probablity of event X_i occuring
# P(X_i = x) = Sum over P(X_i | Z_i = k) * P(Z_i = k)

normal(x, mu, sigma) = 1/(sigma*sqrt(2*pi)) * exp(-(x-mu)**2/(2*sigma**2))

# Here we draw from a bernoulli distribution (biased coin toss) to decide which gaussian distribution to sample from 
# 
# Pi = [0.5, 0.5]
# Z = [1, 2]
mix1(x) = (rand(0) <= 0.5) ? normal(x, 9, 1) : normal(x, 20, 1)

# Pi = [0.75, 0.25]
# Z = [1, 2]
mix2(x) = (rand(0) <= 0.75) ? normal(x, 5, 2.5) : normal(x, 10, 2.5)

plot mix1(x), mix2(x)

pause -1
