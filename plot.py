#!/usr/bin/env python

import pylab as pl
import numpy as np
import os
import struct
import argparse
import glob
import sys
import math

class patch_data:
    def __init__(self):
        self.X = []
        self.Y = []
        self.V = []
       

def nice_axes(a, b):
    p = math.floor(math.log10(b - a))
    a = nice_lower(a, p)
    b = nice_upper(b, p)
    return a, b

def nice_upper(a, p):
    a = a / 10**p
    a = math.ceil(a)
    a = a * 10**p
    return a

def nice_lower(a, p):
    a = a / 10**p
    a = math.floor(a)
    a = a * 10**p
    return a


def read_array(f):
    N = struct.unpack('L', f.read(8))
    N = N[0]


    shape = struct.unpack('L'*N, f.read(8*N))
    shape = np.array(shape)

    size = np.prod(shape)

    print "N:    ", N
    print "shape:",shape
    print "size: ",size

    v = struct.unpack('d'*size, f.read(8*size))

    return np.reshape(v, shape)

def do_file(filename):

    f = open(filename, 'rb')

    pd = patch_data()
    pd.name = filename
    
    vmax = -1E37
    vmin = 1E37

    while True:
        x = y = v = None
        try:
            normal = struct.unpack('i', f.read(4))[0]
            x = read_array(f)
            y = read_array(f)
            v = read_array(f)
            
            #v = np.transpose(v)

            pd.X.append(x)
            pd.Y.append(y)
            pd.V.append(v)
            
            vmax = np.max(v) if np.max(v) > vmax else vmax
            vmin = np.min(v) if np.min(v) < vmin else vmin
        except:
            print sys.exc_info()[0]
            break

    return vmin,vmax,pd

def plot_file(pd,a,b):
    
    fig = pl.figure()
    
    ax = fig.add_subplot(111)
    ax.set_xlabel(pd.name)

    clrrng = np.linspace(a,b,11)
    
    print 'success'

    #print np.shape(x)
    #print np.shape(y)
    #print np.shape(v)

    #print x
    #print y
    #print v
    
    for x,y,v in zip(pd.X,pd.Y,pd.V):
        con = ax.contourf(x,y,v,clrrng)

    pl.colorbar(con, ax = ax)

############

parser = argparse.ArgumentParser()
parser.add_argument('files', nargs='*')
args = parser.parse_args()

print args.files

vmax = -1E37
vmin = 1E37

patch_datas = []

for f in args.files:
    
    fmin,fmax,pd = do_file(f)

    patch_datas.append(pd)

    vmax = fmax if fmax > vmax else vmax
    vmin = fmin if fmin < vmin else vmin

a,b = nice_axes(vmin,vmax)


for pd in patch_datas:
    plot_file(pd,vmin,vmax)



pl.show()










