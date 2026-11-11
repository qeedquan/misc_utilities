import matplotlib.pyplot as plt
import cartopy
import cartopy.crs as ccrs
import cartopy.io.img_tiles as cimgt
import numpy as np

fig = plt.figure(figsize=(14, 8))
ax = fig.add_subplot(1, 1, 1, projection=ccrs.Robinson())
ax.set_global()
ax.gridlines(draw_labels=True, linestyle='--')

# Natural earth land
ax.coastlines(resolution='50m', color='darkgreen', linewidth=0.8)
ax.add_feature(cartopy.feature.LAND, facecolor='#7fbf7f')
ax.add_feature(cartopy.feature.OCEAN, facecolor='#a8d8ea')
ax.add_feature(cartopy.feature.LAKES, facecolor='#a8d8ea')
ax.set_title("World Map", fontsize=16, pad=10)

plt.tight_layout()
plt.savefig("world_map.png", dpi=200)
plt.show()

