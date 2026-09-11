from collections import Counter
import matplotlib.pyplot as plt

# 1. Sample text
text = "python is fun and python is easy to learn and fun to use"

# 2. Clean and split words
words = text.lower().split()

# 3. Count word frequencies
word_counts = Counter(words)

# Get the 5 most common words
common_words = word_counts.most_common(5)
word_list = [item[0] for item in common_words]
count_list = [item[1] for item in common_words]

# 4. Create a bar chart
plt.bar(word_list, count_list, color='skyblue')
plt.xlabel('Words')
plt.ylabel('Frequency')
plt.title('Top 5 Most Common Words')
plt.show()
