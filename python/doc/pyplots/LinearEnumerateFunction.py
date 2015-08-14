import matplotlib.pyplot as plt

# Create the figure
plt.figure(1, figsize=(4, 4))
ax = plt.subplot(111)

# Create the points
ax.plot([0, 0, 1, 0, 1, 2, 3, 2, 1, 0], [
        0, 1, 0, 2, 1, 0, 0, 1, 2, 3], "o", markersize=9)

# Create the arrows
ax.annotate("",
            xy=(0.97, 0), xycoords='data',
            xytext=(0, 0), textcoords='data',
            arrowprops=dict(arrowstyle="-|>", linestyle="dashed", mutation_scale=15,
                            connectionstyle="arc3", color='black'),
            )

ax.arrow(1, 0, -0.97, 0.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)
ax.arrow(0, 1, 1.97, -0.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)
ax.arrow(2, 0, -1.97, 1.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)
ax.arrow(0, 2, 2.97, -1.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)
ax.arrow(3, 0, -2.97, 2.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)
ax.arrow(0, 3, 1.97, -0.97, head_width=0.08, head_length=0.08, fc='k',
         ec='k', length_includes_head=True, linestyle="dashed",)

# Annotate points
ax.annotate('4',
            xy=(1, 1), xycoords='data',
            xytext=(-20, -5), textcoords='offset points', fontsize=16)

ax.annotate('7',
            xy=(2, 1), xycoords='data',
            xytext=(+15, +0), textcoords='offset points', fontsize=16)

ax.annotate('8',
            xy=(1, 2), xycoords='data',
            xytext=(+15, +0), textcoords='offset points', fontsize=16)

# Add labels
ax.annotate(r'$\tau_1$', xy=(1, 0), xytext=(10, 10), ha='left', va='center',
            xycoords='axes fraction', textcoords='offset points', fontsize=20)

ax.annotate(r'$\tau_2$', xy=(0, 1), xytext=(0, 10), ha='left', va='center',
            xycoords='axes fraction', textcoords='offset points', fontsize=20)

# Hide spines
ax.spines['right'].set_color('none')
ax.spines['top'].set_color('none')

# Set spines's position
ax.xaxis.set_ticks_position('bottom')
ax.spines['bottom'].set_position(('data', 0))
ax.yaxis.set_ticks_position('left')
ax.spines['left'].set_position(('data', 0))

# Add labels
plt.xticks([-0.1] + list(range(4)) + [3.2])
ax.set_xticklabels(('', '$0$', '$1$', '$3$', '$6$'), fontsize=20)
plt.yticks([-0.1] + list(range(4)) + [3.2])
ax.set_yticklabels(('', '', '$2$', '$5$', '$9$'), fontsize=20)

# Show the figure
plt.show()
