#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Mar 25 01:06:35 2019

@author: manalipatil
"""

import numpy as np
import matplotlib.pyplot as plt

data_file = np.loadtxt('Final.txt',delimiter=' ')

datarate = data_file[0:10,0]

nocompression = data_file[0:10,1]

compression = data_file[10:20,1]

my_data = np.vstack((datarate,nocompression.T,compression))

my_data = my_data.T

np.savetxt('export.csv',my_data,delimiter=' ')

plt.plot(datarate,nocompression,'or--')
plt.plot(datarate,compression,'ob--')
plt.legend(['No Compression','Compression'])
plt.ylabel('Delta Time milliseconds')
plt.xlabel('Data Rate')
plt.show()
