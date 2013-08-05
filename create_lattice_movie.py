#!/usr/bin/env python

import sys
from optparse import OptionParser
import os   
#from pylab import *
import types
import subprocess

import matplotlib
matplotlib.use('cairo')

import matplotlib.pyplot as plt
import matplotlib.collections as col
import matplotlib.patches as mpatches
import matplotlib.font_manager as fm

import numpy as np
import unicodedata

#import matplotlib.ticker as tkr

u_deplete_history = []
u_neutral_history = []
u_conserve_history = []
r_deplete_history = []
r_neutral_history = []
r_conserve_history = []
p_history = []
sweep_history = []

#for testing
'''
u_deplete_history   = (np.ones(600)*0.2).tolist()
u_neutral_history   = (np.ones(600)*0.4).tolist()
u_conserve_history  = (np.ones(600)*0.6).tolist()
r_deplete_history   = (np.ones(600)*0.2).tolist()
r_neutral_history   = (np.ones(600)*0.4).tolist()
r_conserve_history  = (np.ones(600)*0.6).tolist()
p_history           = (np.ones(600)*0.5).tolist()
sweep_history       = range(600)
'''

def get_refcounts():
    d = {}
    sys.modules
    # collect all classes
    for m in sys.modules.values():
        for sym in dir(m):
            o = getattr (m, sym)
            if type(o) is types.ClassType:
                d[o] = ( sys.getrefcount (o), sys.getsizeof(o) )
    # sort by refcount
    pairs = map (lambda x: (x[1],x[0]), d.items())
    pairs.sort()
    pairs.reverse()
    return pairs

def print_top_10():
    for n, c in get_refcounts()[:10]:
        print '%10d %10d %s' % (n[0], n[1], c.__name__)

def average_states(user,resource):
    
    num_user_deplete        = np.where(np.reshape(user,user.size) < 0.0)[0].size
    num_user_neutral        = np.where(np.reshape(user,user.size) == 0.0)[0].size
    num_user_conserve       = np.where(np.reshape(user,user.size) > 0.0)[0].size
    num_resource_deplete    = np.where(np.reshape(resource,resource.size) < 0.0)[0].size
    num_resource_neutral    = np.where(np.reshape(resource,resource.size) == 0.0)[0].size
    num_resource_conserve   = np.where(np.reshape(resource,resource.size) > 0.0)[0].size
    parity_sum              = 0.0
    
    it = np.nditer(user, flags=['multi_index'])
    while not it.finished:
        if user[it.multi_index] < 0 and resource[it.multi_index] < 0:
            parity_sum += 1
        elif user[it.multi_index] > 0 and resource[it.multi_index] > 0:
            parity_sum += 1
        elif user[it.multi_index] == 0 and resource[it.multi_index] == 0:
            parity_sum += 1
        it.iternext()
    
    counts = map(lambda x: x/float(user.size), map(float, [num_user_deplete, num_user_neutral, num_user_conserve, num_resource_deplete, num_resource_neutral, num_resource_conserve, parity_sum]))
    
    return counts
        
def plot_states(user, resource, min_resource, max_resource, sweep, model_params, file_location, plot_sweep_window=1000):

    plot_fontsize = 12.0
    plot_title_fontsize = 36.0
    label_fontsize = 24.0
    
    plot_tick_fontprop = fm.FontProperties(fname='/System/Library/Fonts/HelveticaNeue.dfont', stretch='condensed', weight='ultralight', style='italic')
    plot_title_fontprop = fm.FontProperties(fname='/System/Library/Fonts/HelveticaNeue.dfont', weight='medium', style='italic')
    label_fontprop = fm.FontProperties(fname='/System/Library/Fonts/HelveticaNeue.dfont', weight='medium', style='italic')
    plot_annotation_fontprop = fm.FontProperties(fname='/System/Library/Fonts/HelveticaNeue.dfont', stretch='condensed', weight='bold')
    
    plot_title_fontcolor = '0.3'
    
    lattice_cmap = plt.get_cmap('YlOrRd_r')
    
    #maps = sorted(m for m in plt.cm.datad if not m.endswith("_r"))
    #nmaps = len(maps) + 1
    #for i,m in enumerate(maps):
        #lattice_cmap = plt.get_cmap(m)
        #plt.figtext(0, 0, m, fontsize=10, ha='left', va='bottom')
    grid_size = 0.75
    
    total_width     = 14.222
    total_height    = 10.667
    left_margin     = 0.5
    right_margin    = 0.5
    top_margin      = 0.5
    bottom_margin   = 0.5
    
    lattice_width       = lattice_height = 4.71
    lattice_title_left  = 7.3364
    lattice_fw = 2.0
    
    plot_width          = 5.585
    plot_height         = 2.7742
    plot_title_left     = 0.5
    
    plot_lw = 3.0
    plot_fw = 2.0
    plot_annotate_el = matplotlib.patches.Ellipse((2, -1), 0.5, 0.5)
    plot_annotate_fontsize = 9.0
    
    label_width     = 0.5
    label_height    = 3.2233
    label_center      = 13.47
    label_left = 13.22
    c_label_bottom  = 6.9467
    n_label_bottom  = 3.7233
    d_label_bottom  = 0.5
    
    c_label_text_color = 'k'
    n_label_text_color = 'white'
    d_label_text_color = 'white'
    
    u_lattice_bottom    = 5.46
    u_lattice_left      = 8.01
    
    r_lattice_bottom    = 0.5
    r_lattice_left      = 8.01
    
    u_plot_bottom       = 7.3958
    u_plot_left         = 1.1472
    
    p_plot_bottom       = 3.9479
    p_plot_left         = 1.149
    p_plot_line_color   = '0.5'
    
    r_plot_bottom       = 0.5
    r_plot_left         = 1.1472
    
    global u_deplete_history
    global u_neutral_history
    global u_conserve_history
    global r_deplete_history
    global r_neutral_history
    global r_conserve_history
    global p_history
    global sweep_history
    
    fig = plt.figure(figsize=(14.222,10.667))
    
    beta_1_string = u'\u03b2' + r'$_1$'
    beta_2_string = u'\u03b2' + r'$_2$'
    
    title_string = r'n:%i %s:%.1f %s:%.1f $f_u$:%.1f $f_r$:%.1f'%(model_params[0], beta_1_string, model_params[1], beta_2_string, model_params[2], model_params[3], model_params[4])
    
    #GREEK SMALL LETTER BETA
    #Unicode: U+03B2, UTF-8: CE B2
    
    '''
    fig.text(0.05, 0.94, 'sweep:%i'%sweep, ha="left", fontsize=12, family=font_family, weight='bold')
    fig.text(0.94, 0.94, title_string, ha="right", fontsize=10, family=font_family)
    '''
#create the lattice state labels
    label_ax = fig.add_axes([label_left/total_width, d_label_bottom/total_height, label_width/total_width, 3.0*label_height/total_height])
    #c_label_ax = fig.add_axes([label_left/total_width, c_label_bottom/total_height, label_width/total_width, label_height/total_height])
    #n_label_ax = fig.add_axes([label_left/total_width, n_label_bottom/total_height, label_width/total_width, label_height/total_height])
    #d_label_ax = fig.add_axes([label_left/total_width, d_label_bottom/total_height, label_width/total_width, label_height/total_height])
    
    #print lattice_cmap(0), lattice_cmap(1)
    #c_label_ax.set_axis_bgcolor(lattice_cmap(1))
    #n_label_ax.set_axis_bgcolor(lattice_cmap(0))
    #d_label_ax.set_axis_bgcolor(lattice_cmap(0))
    
    for item in label_ax.xaxis.get_ticklabels() + label_ax.yaxis.get_ticklabels() + label_ax.xaxis.get_ticklines() + label_ax.yaxis.get_ticklines():
        item.set_alpha(0)
    
    for spine in label_ax.spines.itervalues():
        spine.set_alpha(0)

    patches =   [
                    mpatches.Rectangle((0, 0), 1, 1.0/3.0, ec='none'),
                    mpatches.Rectangle((0, 1.0/3.0), 1, 1.0/3.0, ec='none'),
                    mpatches.Rectangle((0, 2.0/3.0), 1, 1.0/3.0, ec='none')
                ]
    collection = col.PatchCollection(patches, cmap=lattice_cmap, edgecolor='none')
    collection.set_array(np.array([0,1,2]))
    label_ax.add_collection(collection)
    
    bottom_pad = 0.1
    c_adj = 0
    n_adj = -0.02
    d_adj = -0.05
    
    plt.figtext((label_center + c_adj)/total_width, (c_label_bottom + bottom_pad)/total_height, 'conserve', fontproperties=label_fontprop, va='bottom', ha='center', rotation=90, size=label_fontsize, color=c_label_text_color)
    plt.figtext((label_center + n_adj)/total_width, (n_label_bottom + bottom_pad)/total_height, 'neutral', fontproperties=label_fontprop, va='bottom', ha='center', rotation=90, size=label_fontsize, color=n_label_text_color)
    plt.figtext((label_center + d_adj)/total_width, (d_label_bottom + bottom_pad)/total_height, 'deplete', fontproperties=label_fontprop, va='bottom', ha='center', rotation=90, size=label_fontsize, color=d_label_text_color)
    
#plot the user lattice [left,bottom,width,height]
    u_lattice_ax = fig.add_axes([u_lattice_left/total_width, u_lattice_bottom/total_height, lattice_width/total_width, lattice_height/total_height])
    
    #user_ax = plt.axes([-0.12,0.16,grid_size,grid_size])
    
    u_lattice_ax.set_xticks([])
    u_lattice_ax.set_yticks([])
    
    u_lattice_norm = matplotlib.colors.Normalize(vmin=-1, vmax=1)
    
    plt.imshow(user, interpolation='nearest', cmap=lattice_cmap, norm=u_lattice_norm)
    plt.grid(False)
    
    for spine in u_lattice_ax.spines.itervalues():
        spine.set_lw(lattice_fw)
    
    plt.figtext(lattice_title_left/total_width, u_lattice_bottom/total_height, 'user lattice', fontproperties=plot_title_fontprop, va='bottom', ha='left', rotation=90, size=plot_title_fontsize, color=plot_title_fontcolor)

#plot the resource lattice [left,bottom,width,height]
    r_lattice_ax = fig.add_axes([r_lattice_left/total_width, r_lattice_bottom/total_height, lattice_width/total_width, lattice_height/total_height])
    
    r_lattice_ax.set_xticks([])
    r_lattice_ax.set_yticks([])
    
    r_lattice_norm = matplotlib.colors.Normalize(vmin=min_resource, vmax=max_resource)
    
    plt.imshow(resource, interpolation='nearest', cmap=lattice_cmap, norm=r_lattice_norm)
    plt.grid(False)
    
    for spine in r_lattice_ax.spines.itervalues():
        spine.set_lw(lattice_fw)
        
    plt.figtext(lattice_title_left/total_width, r_lattice_bottom/total_height, 'resource lattice', fontproperties=plot_title_fontprop, va='bottom', ha='left', rotation=90, size=plot_title_fontsize, color=plot_title_fontcolor)

#prepare the three plots
    u_deplete, u_neutral, u_conserve, r_deplete, r_neutral, r_conserve, p = average_states(user,resource)
    u_deplete_history.append(   u_deplete)
    u_neutral_history.append(   u_neutral)
    u_conserve_history.append(  u_conserve)
    r_deplete_history.append(   r_deplete)
    r_neutral_history.append(   r_neutral)
    r_conserve_history.append(  r_conserve)
    p_history.append(           p)
    #the + 600 is here for testing
    sweep_history.append(       sweep)
    
    if (sweep_history[-1] - sweep_history[0]) > int(float(plot_sweep_window)*4.0/5.0):
        u_deplete_history.pop(0)
        u_neutral_history.pop(0)
        u_conserve_history.pop(0)
        r_deplete_history.pop(0)
        r_neutral_history.pop(0)
        r_conserve_history.pop(0)
        p_history.pop(0)
        sweep_history.pop(0)
        
    curr_plot_left = sweep_history[0]
    curr_plot_right = sweep_history[0] + plot_sweep_window
    
#plot the user state fractions [left,bottom,width,height]
    u_plot_ax = fig.add_axes([u_plot_left/total_width, u_plot_bottom/total_height, plot_width/total_width, plot_height/total_height])
    
    u_plot_ax.set_xlim((curr_plot_left, curr_plot_right))
    u_plot_ax.set_ylim((0, 1))
    plt.plot(sweep_history, u_deplete_history, color=lattice_cmap(0.0), lw=plot_lw)
    plt.plot(sweep_history, u_neutral_history, color=lattice_cmap(0.5), lw=plot_lw)
    plt.plot(sweep_history, u_conserve_history, color=lattice_cmap(1.0), lw=plot_lw)
    
    u_plot_ax.add_patch(plot_annotate_el)
    u_plot_ax.annotate('%.2f'%u_deplete_history[-1], xy=(sweep_history[-1], u_deplete_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, u_deplete_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=d_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(0.0), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(0.0), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    u_plot_ax.annotate('%.2f'%u_neutral_history[-1], xy=(sweep_history[-1], u_neutral_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, u_neutral_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=d_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(0.5), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(0.5), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    u_plot_ax.annotate('%.2f'%u_conserve_history[-1], xy=(sweep_history[-1], u_conserve_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, u_conserve_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=c_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(1.0), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(1.0), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    
    for label in u_plot_ax.xaxis.get_ticklabels() + u_plot_ax.yaxis.get_ticklabels():
        label.set_fontproperties(plot_tick_fontprop)
        label.set_fontsize(plot_fontsize)

    for tick in u_plot_ax.xaxis.get_ticklines() + u_plot_ax.yaxis.get_ticklines():
        tick.set_lw(plot_fw)
        tick.set_alpha(0.5)

    for spine in u_plot_ax.spines.itervalues():
        spine.set_lw(plot_fw)

    plt.figtext(plot_title_left/total_width, u_plot_bottom/total_height, 'user', fontproperties=plot_title_fontprop, va='bottom', ha='right', rotation=90, size=plot_title_fontsize, color=plot_title_fontcolor)

#plot the lattice parity [left,bottom,width,height]
    p_plot_ax = fig.add_axes([p_plot_left/total_width, p_plot_bottom/total_height, plot_width/total_width, plot_height/total_height])
    
    p_plot_ax.set_xlim((curr_plot_left, curr_plot_right))
    p_plot_ax.set_ylim((0, 1))
    plt.plot(sweep_history, p_history, color=p_plot_line_color, lw=plot_lw)

    p_plot_ax.add_patch(plot_annotate_el)
    p_plot_ax.annotate('%.2f'%p_history[-1], xy=(sweep_history[-1], p_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, p_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=d_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=p_plot_line_color, ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=p_plot_line_color, ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    
    for label in p_plot_ax.xaxis.get_ticklabels() + p_plot_ax.yaxis.get_ticklabels():
        label.set_fontproperties(plot_tick_fontprop)
        label.set_fontsize(plot_fontsize)

    for tick in p_plot_ax.xaxis.get_ticklines() + p_plot_ax.yaxis.get_ticklines():
        tick.set_lw(plot_fw)
        tick.set_alpha(0.5)

    for spine in p_plot_ax.spines.itervalues():
        spine.set_lw(plot_fw)
    
    plt.figtext(plot_title_left/total_width, p_plot_bottom/total_height, 'parity', fontproperties=plot_title_fontprop, va='bottom', ha='right', rotation=90, size=plot_title_fontsize, color=plot_title_fontcolor)

#plot the resource state fractions [left,bottom,width,height]
    r_plot_ax = fig.add_axes([r_plot_left/total_width, r_plot_bottom/total_height, plot_width/total_width, plot_height/total_height])
    
    r_plot_ax.set_xlim((curr_plot_left, curr_plot_right))
    r_plot_ax.set_ylim((0, 1))
    plt.plot(sweep_history, r_deplete_history, color=lattice_cmap(0.0), lw=plot_lw)
    plt.plot(sweep_history, r_neutral_history, color=lattice_cmap(0.5), lw=plot_lw)
    plt.plot(sweep_history, r_conserve_history, color=lattice_cmap(1.0), lw=plot_lw)

    r_plot_ax.add_patch(plot_annotate_el)
    r_plot_ax.annotate('%.2f'%r_deplete_history[-1], xy=(sweep_history[-1], r_deplete_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, r_deplete_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=d_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(0.0), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(0.0), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    r_plot_ax.annotate('%.2f'%r_neutral_history[-1], xy=(sweep_history[-1], r_neutral_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, r_neutral_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=d_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(0.5), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(0.5), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )
    r_plot_ax.annotate('%.2f'%r_conserve_history[-1], xy=(sweep_history[-1], r_conserve_history[-1]),  xycoords='data',
        xytext=(sweep_history[-1] + plot_sweep_window*0.05, r_conserve_history[-1]), textcoords='data',
        fontsize=plot_annotate_fontsize, va="center",
        color=c_label_text_color,
        fontproperties=plot_annotation_fontprop,
        bbox=dict(boxstyle="round", fc=lattice_cmap(1.0), ec="none"),
        arrowprops=dict(arrowstyle="wedge,tail_width=1.",
                        fc=lattice_cmap(1.0), ec="none",
                        patchA=None,
                        patchB=plot_annotate_el,
                        relpos=(0.2, 0.5),
                        )
        )

    for label in r_plot_ax.xaxis.get_ticklabels() + r_plot_ax.yaxis.get_ticklabels():
        label.set_fontproperties(plot_tick_fontprop)
        label.set_fontsize(plot_fontsize)

    for tick in r_plot_ax.xaxis.get_ticklines() + r_plot_ax.yaxis.get_ticklines():
        tick.set_lw(plot_fw)
        tick.set_alpha(0.5)

    for spine in r_plot_ax.spines.itervalues():
        spine.set_lw(plot_fw)

    plt.figtext(plot_title_left/total_width, r_plot_bottom/total_height, 'resource', fontproperties=plot_title_fontprop, va='bottom', ha='right', rotation=90, size=plot_title_fontsize, color=plot_title_fontcolor)





    #for line in u_plot_ax.xaxis.get_ticklines():
    #    line.set_alpha(0)

    #print u_ave_history
    
#plot the user lattice ledgend [left,bottom,width,height]
    '''
    user_ledgend_ax = fig.add_axes([.05, 0.08, 0.41, 0.04])
    user_ledgend_ax.set_xticks([])
    user_ledgend_ax.set_yticks([])
    patches =   [
                    mpatches.Rectangle((0,0),1.0/3.0, 1),
                    mpatches.Rectangle((1.0/3.0,0),1.0/3.0, 1),
                    mpatches.Rectangle((2.0/3.0,0),1.0/3.0, 1)
                ]
    collection = col.PatchCollection(patches, cmap=user_cmap)
    collection.set_array(np.array([0,1,2]))
    user_ledgend_ax.add_collection(collection)

    user_ledgend_ax.text(1.0/6.0, -0.5, "Pig", ha="center", size=7, family=font_family)
    user_ledgend_ax.text(0.5, -0.5, "Swiss", ha="center", size=7, family=font_family)
    user_ledgend_ax.text(5.0/6.0, -0.5, "Camel", ha="center", size=7, family=font_family)
    user_ledgend_ax.text(0.5, -1.2, "User", ha="center", size=10, family=font_family)
    '''
    
#plot the resource lattice legend [left,bottom,width,height]
    '''
    resource_ledgend_ax = fig.add_axes([.53, 0.08, 0.41, 0.04])
    
    resource_ledgend_cb = matplotlib.colorbar.ColorbarBase(resource_ledgend_ax, cmap=resource_cmap,
                                                            norm=resource_norm,
                                                            orientation='horizontal')
    
    #resource_ledgend_ax.set_major_formatter(matplotlib.ticker.FormatStrFormatter('%0.1f'))
    resource_ledgend_cb.set_ticks([min_resource,(min_resource+ max_resource)/2.0, max_resource])
    resource_ledgend_cb.set_ticklabels(['%.2f'%min_resource,'%.2f'%((min_resource+ max_resource)/2.0), '%.2f'%max_resource])
    
    #resource_ledgend_ax.xaxis.set_major_formatter(tkr.FormatStrFormatter('%d'))
    
    for label in resource_ledgend_ax.xaxis.get_ticklabels():
        label.set_fontsize(7)
        label.set_fontname(font_family)
        
    for line in resource_ledgend_ax.xaxis.get_ticklines():
        line.set_alpha(0)
        
    resource_ledgend_cb.ax.set_xlabel('Resource',size=10, position=(0.5,0), ha='center', family=font_family)
    '''
    plt.savefig(file_location, format="png")
    
    #plt.savefig('map_test_%s.png'%m, format="png")
    
    plt.close(fig)
    """
    del user_cmap
    del resource_cmap
    del fig
    del user_ax
    del user_norm
    del user_ledgend_ax
    del resource_ax
    del resource_norm
    del resource_ledgend_ax
    del resource_ledgend_cb
    """
    
def main(argv=None):
    if argv is None:
        argv = sys.argv
    
    parser = OptionParser()
    parser.add_option("-f", "--file",
                      dest="filename", default=None,
                      help="The location of the state output from the CPRM model", metavar="FILE")
    parser.add_option("-s", "--start",
                      dest="start_sweep", default=0,
                      help="The sweep to start the movie from.", metavar="START_SWEEP")
    parser.add_option("-e", "--end",
                      dest="end_sweep", default=None,
                      help="The sweep to end the movie at.", metavar="END_SWEEP")
    parser.add_option("-m", "--movie",
                  action="store_true", dest="movie", default=False,
                  help="dont plot files. only make the movie")
    parser.add_option("-i", "--increment",
                      dest="increment", default=1,
                      help="The sweep increment. 1 means every sweep is plotted. 10 means every 10th sweep is ploted. etc.", metavar="START_SWEEP")
    parser.add_option("-w", "--window",
                      dest="window", default=1000,
                      help="The number of sweeps tp show in the plots.", metavar="START_SWEEP")
    
    (options, args) = parser.parse_args()
    
    user_sweeps = {}
    resource_sweeps = {}
    resource_max = -10000000
    resource_min =  10000000
    
    increment = int(options.increment)
    
    start_sweep = int(options.start_sweep)

    window = int(options.window)
    
    try:
        end_sweep = int(options.end_sweep)
    except:
        end_sweep = None
    
    os.chdir(os.path.dirname(options.filename))
    
    sys_name = os.path.basename(options.filename).split('.dat')[0]
    
    if not os.path.exists("%s_images"%sys_name):
        os.makedirs("%s_images"%sys_name)
    
    model_params = sys_name.split("_")
    
    sweeps                      = int(model_params[3])
    n                           = int(model_params[4])
    beta1                       = float(model_params[5])
    beta2                       = float(model_params[6])
    user_field_strength         = float(model_params[7])
    resource_field_strength     = float(model_params[8])
    resource_field_increment    = float(model_params[9])
    
    print "Model parameters"
    print "    n:                       %i"%n
    print "    beta 1:                  %.2f"%beta1
    print "    beta 2:                  %.2f"%beta2
    print "    user field strength:     %.2f"%user_field_strength
    print "    resource field strength: %.2f"%resource_field_strength
    print "    resource field increment:%.2f"%resource_field_increment
    
    if not options.movie:
        if end_sweep is not None:
            print "Loading from sweep %i to sweep %i"%(start_sweep, end_sweep)
        else:
            print "Loading from sweep %i to the last sweep"%(start_sweep)
        
        curr_sweep = -1
        f = open(options.filename, "r")
        for line_num, line in enumerate(f):
            if line_num == 0:
                grid_width = int(line)
            else:
                #print curr_sweep, " ",
                if (line_num-1)%2 == 0:
                    curr_sweep += 1
                if line.startswith("u") and curr_sweep >= start_sweep:
                    #print curr_sweep, "u"
                    if end_sweep is not None:
                        if curr_sweep <= end_sweep:
                            user_sweeps[curr_sweep] =  np.reshape(np.array(map(float, line.split()[1:])),(grid_width,grid_width)) 
                    else:
                        user_sweeps[curr_sweep] =  np.reshape(np.array(map(float, line.split()[1:])),(grid_width,grid_width)) 
                elif line.startswith("r") and curr_sweep >= start_sweep:
                    #print curr_sweep, "r"
                    if end_sweep is not None:
                        if curr_sweep <= end_sweep:
                            resource_tmp = np.reshape(np.array(map(float, line.split()[1:])),(grid_width,grid_width))
                            resource_sweeps[curr_sweep] = resource_tmp 
                            
                            this_max = np.amax(resource_tmp)
                            this_min = np.amin(resource_tmp)
                            
                            if this_min < resource_min:
                                resource_min = this_min
                            if  this_max > resource_max:
                                resource_max = this_max
                    else:
                        resource_tmp = np.reshape(np.array(map(float, line.split()[1:])),(grid_width,grid_width))
                        resource_sweeps[curr_sweep] = resource_tmp
                        
                        this_max = np.amax(resource_tmp)
                        this_min = np.amin(resource_tmp)
                        
                        if this_min < resource_min:
                            resource_min = this_min
                        if  this_max > resource_max:
                            resource_max = this_max
        #print resource_sweeps
        print "Plotting"
        frame_number = 0
        for sweep, user in user_sweeps.iteritems():
            if (sweep%50 == 0):
                print "    sweep: %i"%sweep
                #print_top_10()
            if (sweep%increment == 0):
                resource = resource_sweeps[sweep]
                plot_states(user,resource,resource_min, resource_max, sweep, (n, beta1, beta2, user_field_strength, resource_field_strength, resource_field_increment) ,"%s_images/%s_%i.png"%(sys_name, sys_name, frame_number),plot_sweep_window=window)
                frame_number += 1
            #print sys.modules.keys()
            #print sys.getrefcount(user), sys.getrefcount(resource)
    
    print "Creating movie"
    proc_args = "ffmpeg -y -r 30 -sameq -i %s_images/%s_%s.png %s.mp4"%(sys_name,sys_name,"%d",sys_name)
    proc = subprocess.Popen(proc_args, shell=True)
    proc.wait()
    #mencoder dvd://1 -o /dev/null -ss 32 -ovc x264 -x264encopts pass=1:turbo:bitrate=900:bframes=1:me=umh:partitions=all:trellis=1:qp_step=4:qcomp=0.7:direct_pred=auto keyint=300 -vf crop=720:352:0:62,scale=-10:-1,harddup -oac faac -faacopts br=192:mpeg=4:object=2 -channels 2 -srate 48000 -ofps 24000/1001
    #mencoder dvd://1 -o narnia.avi -ss 32 -ovc x264 -x264encopts pass=2:turbo:bitrate=900:frameref=5:bframes=1:me=umh:partitions=all:trellis=1:qp_step=4:qcomp=0.7:direct_pred=auto:keyint=300 -vf crop=720:352:0:62,scale=-10:-1,harddup -oac faac -faacopts br=192:mpeg=4:object=2 -channels 2 -srate 48000 -ofps 24000/1001

    #mencoder mf://CPRM_states_100_6.0_1.0_0.1_1.0_1.0_images/*.png -mf fps=30:type=png -ovc x264 -x264encopts bitrate=1200:threads=auto:frameref=5:bframes=0:nob_pyramid:nob_adapt:direct_pred=auto:subq=6:mixed_refs:nodct_decimate:no_psnr -o outputfile.avi
    #mplayer-devel outputfile.avi -dumpvideo -dumpfile outputfile.h264
    #mp4creator -create=outputfile.h264 -rate=30.0 outputfile.mp4
    
    #mencoder mf://PATH/*.png -mf fps=FPS -vf scale=720x576,harddup -ovc x264 -x264encopts bitrate=BITRATE:threads=auto:frameref=5:bframes=0:nob_pyramid:nob_adapt:direct_pred=auto:subq=6:mixed_refs:nodct_decimate:no_psnr -nosound -ofps FPS -noskip -of rawvideo -o output.264
    
    #mencoder mf://CPRM_states_100_6.0_1.0_0.1_1.0_1.0_images/*.png -mf fps=30:type=png -vf harddup -ovc x264 -x264encopts bitrate=1200:threads=auto -nosound -ofps 30 -noskip -of rawvideo -o output.264
    #mencoder mf://CPRM_states_100_6.0_1.0_0.1_1.0_1.0_images/*.png -mf fps=30:type=png -sws 9 -vf harddup -oac faac -faacopts br=128:mpeg=4:object=2:raw -ovc x264 -x264encopts bitrate=900:vbv_maxrate=1500:vbv_bufsize=2000:nocabac:level_idc=13:global_header -of lavf -lavfopts format=mp4 -o output.mp4
    
    #mencoder mf://CPRM_states_100_6.0_1.0_0.1_1.0_1.0_images/*.png -mf fps=30:type=png -o test.avi -ovc lavc -lavcopts vcodec=msmpeg4v2:vbitrate=800
    
    #ffmpeg -y -r 30 -b 64k -i CPRM_states_100_6.0_1.0_0.1_1.0_1.0_images/CPRM_states_100_6.0_1.0_0.1_1.0_1.0_%d.png CPRM_states_100_6.0_1.0_0.1_1.0_1.0.mp4


    #A = np.random.randint(-1,2,size=(100,100))
    #B = np.random.rand(100,100)*100 - 50
    #A = np.ones((100,100))
    #B = np.ones((100,100))
    #plot_states(A,B,-1, 1, 0, "%s_images/test.png"%sys_name)
    
    #print A
if __name__ == "__main__": 
    sys.exit(main())