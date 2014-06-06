#!/usr/bin/env python

from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import matplotlib.pyplot as plt
import numpy as np

#import pylab as pl
import numpy as np
import os
import struct
import argparse
import glob
import sys
import math

import Diffusion2D.unit_vec as uv

class Iter_Data:
    def __init__(self):
        pass

class patch_data:
    def __init__(self):
        self.X = []
        self.Y = []
        self.Z = []
        self.V = []
       

def nice_axes(a, b):
    try:
        p = math.floor(math.log10(b - a))
        A = nice_lower(a, p)
        B = nice_upper(b, p)
    except:
        print "a b"
        print a,b
        raise

    return A, B

def nice_upper(a, p):
    a = a / 10**p
    a = math.ceil(a)
    a = a * 10**p
    return a

def nice_lower(a, p):
    try:
        a = a / 10**p
    except:
        print "a",a,"p",p
        raise
    a = math.floor(a)
    a = a * 10**p
    return a


def read_array(f):
    N = struct.unpack('L', f.read(8))
    N = N[0]

    try:
        shape = struct.unpack('L'*N, f.read(8*N))
    except struct.error as er:
        print "N =",N
        raise
    
    shape = np.array(shape)

    size = np.prod(shape)

    #print "N:    ", N
    #print "shape:",shape
    #print "size: ",size

    v = struct.unpack('d'*size, f.read(8*size))

    return np.reshape(v, shape)

def do_file(filename):

    f = open(filename, 'rb')

    pd = patch_data()
    pd.name = filename
    
    f_max = -1E37
    f_min = 1E37

    while True:
        x = [None]*3
        xdir = [None]*3
        try:
            xdir[0] = struct.unpack('i', f.read(4))[0]
        except struct.error as er:
            print "eof"
            break
        except:
            print sys.exc_info()[0]
            raise
        
        try:
            xdir[1] = struct.unpack('i', f.read(4))[0]
            xdir[2] = struct.unpack('i', f.read(4))[0]
        
            
            #x[0] = read_array(f)
            #x[1] = read_array(f)
            #x[2] = read_array(f)
            
            x[xdir[0]] = read_array(f)
            x[xdir[1]] = read_array(f)
            x[xdir[2]] = read_array(f)
            v = read_array(f)
            
            #v = np.transpose(v)

            pd.X.append(x[0])
            pd.Y.append(x[1])
            pd.Z.append(x[2])
            pd.V.append(v)
            
            f_max = max(np.max(v), f_max)
            f_min = min(np.min(v), f_min)
        except:
            print sys.exc_info()[0]
            raise

    f.close()
 
    print "min max"
    print "{0:16f}{1:16f}".format(f_min,f_max)

    return f_min, f_max, pd

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

def plot3(patch_datas, vmin, vmax):

    fig = plt.figure()
    ax = fig.gca(projection='3d')


    for pd in patch_datas:
        plot3_patch(ax,pd,vmin,vmax)


def plot3_patch(ax,pd,vmin,vmax):

    for x,y,z,v in zip(pd.X,pd.Y,pd.Z,pd.V):
        N = v-vmin # clamp min to 0
        N = N/(vmax-vmin)  # normalize 0..1
        surf = ax.plot_surface(x, y, z, rstride=1, cstride=1, facecolors=cm.jet(N),
			linewidth=0, antialiased=False, shade=False)

############
def program_3d():
    val_max = -100000
    val_min = 100000
    
    #print "vmin vmax"
    #print val_min,val_max
    
    patch_datas = []
    
    for f in args.files:
        
        fmin,fmax,pd = do_file(f)
    
        patch_datas.append(pd)
    
        #print "vmin vmax fmin fmax"
        #print "{0:16}".format(val_min,val_max,fmin,fmax)
    
    
        val_max = max(fmax, val_max)
        val_min = min(fmin, val_min)
    
        #print "vmin vmax"
        #print val_min,val_max
    
    a,b = nice_axes(val_min,val_max)
    
    
    for pd in patch_datas:
        #plot_file(pd,a,b)
        pass
    
    
    
    plot3(patch_datas, val_min,val_max)
    
    
    
    plt.show()

def program_idata(args):

    data = {}

    for filename in args.files:
        f = open(filename, 'rb')
        
        d = Iter_Data()
        
        d.S = read_array(f)
        d.v_0 = read_array(f)

        data[filename] = d


    for k,d in data.items():

        fig = plt.figure()
        
        ax = fig.add_subplot(221)
        ax.plot(d.S,'-o')
        ax.set_xlabel('i')
        ax.set_ylabel('S')

        ax = fig.add_subplot(222)
        ax.plot(d.v_0,'-o')
        ax.set_xlabel('i')
        ax.set_ylabel('v')
       
        ax = fig.add_subplot(223)
        ax.plot(d.S, d.v_0, '-o')
        ax.set_xlabel('S')
        ax.set_ylabel('v')

        

    plt.show()


############

parser = argparse.ArgumentParser()
parser.add_argument('files', nargs='*')
parser.add_argument('-r', action='store_true')
args = parser.parse_args()

if len(args.files) == 0:
    print "no input files"
    sys.exit(1)

#print args.files

if(args.r):
    program_idata(args)
else:
    program_3d()










