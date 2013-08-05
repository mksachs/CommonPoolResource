#!/usr/bin/env python

import sys
from optparse import OptionParser
import os

import matplotlib
#matplotlib.use('cairo')
#matplotlib.use('Agg')

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import axes3d
from matplotlib import cm
from matplotlib.font_manager import FontProperties

import numpy as np
from matplotlib.mlab import griddata


def basic_stats(list):
    #returns (ave, var)
    
    sum = 0
    sum_sq = 0
    for num in list:
        sum += num
        sum_sq += num**2.0
    
    return (sum/float(len(list)), sum_sq/float(len(list)) - (sum/float(len(list)))**2.0)

def average_over_run(user_values, user_conserve_values, user_neutral_values, user_deplete_values, resource_values, resource_conserve_values, resource_neutral_values, resource_deplete_values, parity_values):
    
    user_means = []
    user_vars = []
    user_conserve_means = []
    user_conserve_vars = []
    user_neutral_means = []
    user_neutral_vars = []
    user_deplete_means = []
    user_deplete_vars = []
    resource_means = []
    resource_vars = []
    resource_conserve_means = []
    resource_conserve_vars = []
    resource_neutral_means = []
    resource_neutral_vars = []
    resource_deplete_means = []
    resource_deplete_vars = []
    parity_means = []
    parity_vars = []
    
    for val in user_values:
        user_means.append(abs(val[0]))
        user_vars.append(abs(val[1]))
    
    for val in user_conserve_values:
        user_conserve_means.append(val[0])
        user_conserve_vars.append(val[1])
    
    for val in user_neutral_values:
        user_neutral_means.append(val[0])
        user_neutral_vars.append(val[1])
    
    for val in user_deplete_values:
        user_deplete_means.append(val[0])
        user_deplete_vars.append(val[1])
        
    for val in resource_values:
        resource_means.append(abs(val[0]))
        resource_vars.append(abs(val[1]))
        
    for val in resource_conserve_values:
        resource_conserve_means.append(val[0])
        resource_conserve_vars.append(val[1])
    
    for val in resource_neutral_values:
        resource_neutral_means.append(val[0])
        resource_neutral_vars.append(val[1])
    
    for val in resource_deplete_values:
        resource_deplete_means.append(val[0])
        resource_deplete_vars.append(val[1])
    
    for val in parity_values:
        parity_means.append(val[0])
        parity_vars.append(val[1])
    
    user_mean,user_var                              = basic_stats(user_means)
    user_conserve_mean,user_conserve_var            = basic_stats(user_conserve_means)
    user_neutral_mean,user_neutral_var              = basic_stats(user_neutral_means)
    user_deplete_mean,user_deplete_var              = basic_stats(user_deplete_means)
    resource_mean,resource_var                      = basic_stats(resource_means)
    resource_conserve_mean,resource_conserve_var    = basic_stats(resource_conserve_means)
    resource_neutral_mean,resource_neutral_var      = basic_stats(resource_neutral_means)
    resource_deplete_mean,resource_deplete_var      = basic_stats(resource_deplete_means)
    parity_mean,parity_var                          = basic_stats(parity_means)

    
    user_sample_var_mean, user_sample_var_var                           = basic_stats(user_vars)
    user_conserve_sample_var_mean, user_conserve_sample_var_var         = basic_stats(user_conserve_vars)
    user_neutral_sample_var_mean, user_neutral_sample_var_var           = basic_stats(user_neutral_vars)
    user_deplete_sample_var_mean, user_deplete_sample_var_var           = basic_stats(user_deplete_vars)
    resource_sample_var_mean,resource_sample_var_var                    = basic_stats(resource_vars)
    resource_conserve_sample_var_mean, resource_conserve_sample_var_var = basic_stats(resource_conserve_vars)
    resource_neutral_sample_var_mean, resource_neutral_sample_var_var   = basic_stats(resource_neutral_vars)
    resource_deplete_sample_var_mean, resource_deplete_sample_var_var   = basic_stats(resource_deplete_vars)
    parity_sample_var_mean, parity_sample_var_var   = basic_stats(parity_vars)
    
    
    return {    "user_mean"                         : user_mean,
                "user_var"                          : user_var,
                "user_conserve_mean"                : user_conserve_mean,
                "user_conserve_var"                 : user_conserve_var,
                "user_neutral_mean"                 : user_neutral_mean,
                "user_neutral_var"                  : user_neutral_var,
                "user_deplete_mean"                 : user_deplete_mean,
                "user_deplete_var"                  : user_deplete_var,
                "resource_mean"                     : resource_mean,
                "resource_var"                      : resource_var,
                "resource_conserve_mean"            : resource_conserve_mean,
                "resource_conserve_var"             : resource_conserve_var,
                "resource_neutral_mean"             : resource_neutral_mean,
                "resource_neutral_var"              : resource_neutral_var,
                "resource_deplete_mean"             : resource_deplete_mean,
                "resource_deplete_var"              : resource_deplete_var,
                "parity_mean"                       : parity_mean,
                "parity_var"                        : parity_var,
                "user_sample_var_mean"              : user_sample_var_mean,
                "user_sample_var_var"               : user_sample_var_var,
                "user_conserve_sample_var_mean"     : user_conserve_sample_var_mean,
                "user_conserve_sample_var_var"      : user_conserve_sample_var_var,
                "user_neutral_sample_var_mean"      : user_neutral_sample_var_mean,
                "user_neutral_sample_var_var"       : user_neutral_sample_var_var,
                "user_deplete_sample_var_mean"      : user_deplete_sample_var_mean,
                "user_deplete_sample_var_var"       : user_deplete_sample_var_var,
                "resource_sample_var_mean"          : resource_sample_var_mean,
                "resource_sample_var_var"           : resource_sample_var_var,
                "resource_conserve_sample_var_mean" : resource_conserve_sample_var_mean,
                "resource_conserve_sample_var_var"  : resource_conserve_sample_var_var,
                "resource_neutral_sample_var_mean"  : resource_neutral_sample_var_mean,
                "resource_neutral_sample_var_var"   : resource_neutral_sample_var_var,
                "resource_deplete_sample_var_mean"  : resource_deplete_sample_var_mean,
                "resource_deplete_sample_var_var"   : resource_deplete_sample_var_var,
                "parity_sample_var_mean"            : parity_sample_var_mean,
                "parity_sample_var_var"             : parity_sample_var_var
                }
    
def main(argv=None):
    if argv is None:
        argv = sys.argv
    
    parser = OptionParser()
    parser.add_option("-f", "--file",
                      dest="filename", default=None,
                      help="The location of the sweep output from the CPRM model", metavar="FILE")
    parser.add_option("-a", "--view-angle",
                      dest="view_angle", default=120,
                      help="The view angle of the 3D plots for a two parameter sweep")
    parser.add_option("--p1",
                      dest="p1", default=None,
                      help="The value of the first parameter at which to plot a 2d slice")
    parser.add_option("--p2",
                      dest="p2", default=None,
                      help="The value of the second parameter at which to plot a 2d slice")
    parser.add_option("-c",
                      dest="clean_3d", action="store_true", default=False,
                      help="If this option is included the fixed line that defines the 2d plot will not be plotted on the 3d plots.")
    parser.add_option("-i",
                      dest="print_info", action="store_true", default=False,
                      help="If this option is enabled the system will print out the value of all of the parameters at the fixed values of --p1 and --p2. No plot will be generated.")
    parser.add_option("-s",
                      dest="slice", action="store_true", default=False,
                      help="If this option is enabled the system will print out a one parameter sweep at fixed --p1 or --p2. No plot will be generated. Set --p1 p2 to plot along the p1 = p2 axis.")
    
    (options, args) = parser.parse_args()
    
    os.chdir(os.path.dirname(options.filename))
    
    sys_name = os.path.basename(options.filename).split('.dat')[0]
    
    model_params = sys_name.split("_")
    
    sweep_type                  = model_params[1]
    if sweep_type == "OPS":
        swept_params                = model_params[2]
        n                           = int(model_params[4])
        beta1                       = float(model_params[5])
        beta2                       = float(model_params[6])
        user_field_strength         = float(model_params[7])
        resource_field_strength     = float(model_params[8])
        resource_field_increment    = float(model_params[9])
    elif sweep_type == "TPS":
        swept_params                = (model_params[2], model_params[3])
        n                           = int(model_params[5])
        beta1                       = float(model_params[6])
        beta2                       = float(model_params[7])
        user_field_strength         = float(model_params[8])
        resource_field_strength     = float(model_params[9])
        resource_field_increment    = float(model_params[10])
    
    print "Sweep parameters"
    print "    sweep type:              %s"%sweep_type
    if sweep_type == "OPS":
        print "    swept parameter:        %s"%swept_params
    elif sweep_type == "TPS":
        print "    swept parameters:        %s %s"%swept_params
    print "    n:                       %i"%n
    print "    beta 1:                  %.2f"%beta1
    print "    beta 2:                  %.2f"%beta2
    print "    user field strength:     %.2f"%user_field_strength
    print "    resource field strength: %.2f"%resource_field_strength
    print "    resource field increment:%.2f"%resource_field_increment
    
    all_params = {'n':n,'beta1':beta1,'beta2':beta2,'UFS':user_field_strength, 'RFS':resource_field_strength,'resource_field_increment':resource_field_increment}
    
    sweeps = []
    
    f = open(options.filename, "r")
    for line_num, line in enumerate(f):
        if line_num == 0:
            col_titles = line.split()
        else:
            line_split = map(float,line.split())
            current_sweep = {}
            for col_num, col_title in enumerate(col_titles):
                current_sweep[col_title] = line_split[col_num]
            
            sweeps.append(current_sweep)
    
    sweep_values = []
    
    for sweep_num, sweep in enumerate(sweeps):
        if sweep["run"] == 0:
            if sweep_num != 0:
                run_result = average_over_run(user_values, user_conserve_values, user_neutral_values, user_deplete_values, resource_values, resource_conserve_values, resource_neutral_values, resource_deplete_values, parity_values)
                run_result["beta1"] = curr_beta1
                run_result["beta2"] = curr_beta2
                run_result["UFS"] = curr_user_field_strength
                run_result["RFS"] = curr_resource_field_strength
                run_result["resource_field_increment"] = curr_resource_field_increment
                
                sweep_values.append(run_result)
                    
                    
            #start new set of averages
            curr_beta1                      = sweep["beta1"]
            curr_beta2                      = sweep["beta2"]
            curr_user_field_strength        = sweep["user_field_strength"]
            curr_resource_field_strength    = sweep["resource_field_strength"]
            curr_resource_field_increment   = sweep["resource_field_increment"]
            
            user_values = []
            user_conserve_values = []
            user_neutral_values = []
            user_deplete_values = []
            resource_values = []
            resource_conserve_values = []
            resource_neutral_values = []
            resource_deplete_values = []
            parity_values = []

        user_samples = []
        user_conserve_samples = []
        user_neutral_samples = []
        user_deplete_samples = []
        resource_samples = []
        resource_conserve_samples = []
        resource_neutral_samples = []
        resource_deplete_samples = []
        parity_samples = []
        for sample in range(int(float(len(sweep) - 6.0)/5.0)):
            user_samples.append(sweep["sample%i_user"%(sample+1)])
            user_conserve_samples.append(sweep["sample%i_user_conserve_fraction"%(sample+1)])
            user_neutral_samples.append(sweep["sample%i_user_neutral_fraction"%(sample+1)])
            user_deplete_samples.append(sweep["sample%i_user_deplete_fraction"%(sample+1)])
            resource_samples.append(sweep["sample%i_resource"%(sample+1)])
            resource_conserve_samples.append(sweep["sample%i_resource_conserve_fraction"%(sample+1)])
            resource_neutral_samples.append(sweep["sample%i_resource_neutral_fraction"%(sample+1)])
            resource_deplete_samples.append(sweep["sample%i_resource_deplete_fraction"%(sample+1)])
            parity_samples.append(sweep["sample%i_parity"%(sample+1)])
            #print sweep["sample%i_user"%(sample+1)], sweep["sample%i_resource"%(sample+1)]
        
        user_values.append(basic_stats(user_samples))
        user_conserve_values.append(basic_stats(user_conserve_samples))
        user_neutral_values.append(basic_stats(user_neutral_samples))
        user_deplete_values.append(basic_stats(user_deplete_samples))
        resource_values.append(basic_stats(resource_samples))
        resource_conserve_values.append(basic_stats(resource_conserve_samples))
        resource_neutral_values.append(basic_stats(resource_neutral_samples))
        resource_deplete_values.append(basic_stats(resource_deplete_samples))
        parity_values.append(basic_stats(parity_samples))
        
        if sweep_num == len(sweeps) - 1:
            run_result = average_over_run(user_values, user_conserve_values, user_neutral_values, user_deplete_values, resource_values, resource_conserve_values, resource_neutral_values, resource_deplete_values, parity_values)
            run_result["beta1"] = curr_beta1
            run_result["beta2"] = curr_beta2
            run_result["UFS"] = curr_user_field_strength
            run_result["RFS"] = curr_resource_field_strength
            run_result["resource_field_increment"] = curr_resource_field_increment
            
            sweep_values.append(run_result)
        
    if sweep_type == "OPS":
        for val in sweep_values:
            print val["beta1"], val["user_mean"], val["user_var"], val["user_conserve_mean"], val["user_conserve_var"], val["user_neutral_mean"], val["user_neutral_var"], val["user_deplete_mean"], val["user_deplete_var"], val["resource_mean"], val["resource_var"], val["resource_conserve_mean"], val["resource_conserve_var"], val["resource_neutral_mean"], val["resource_neutral_var"], val["resource_deplete_mean"], val["resource_deplete_var"]
    elif sweep_type == "TPS":
        param1_name = swept_params[0].split("-")[0]
        param2_name = swept_params[1].split("-")[0]
        
        #print param1_name, param2_name
        
        param1_end_val = float(swept_params[0].split("-")[1])
        param2_end_val = float(swept_params[1].split("-")[1])
        
        title_string = r''
        
        if param1_name == 'beta1':
            param1_display_name = r'$\beta_u$'
        elif param1_name == 'beta2':
            param1_display_name = r'$\beta_r$'
        elif param1_name == 'RFS':
            param1_display_name = r'$f_r$'
        elif param1_name == 'UFS':
            param1_display_name = r'$f_u$'
        else:
            param1_display_name = param1_name
        
        if param2_name == 'beta1':
            param2_display_name = r'$\beta_u$'
        elif param2_name == 'beta2':
            param2_display_name = r'$\beta_r$'
        elif param2_name == 'RFS':
            param2_display_name = r'$f_r$'
        elif param2_name == 'UFS':
            param2_display_name = r'$f_u$'
        else:
            param2_display_name = param2_name
        
        title_string = r'%s:%.1f-%.1f %s:%.1f-%.1f '%(param1_display_name, all_params[param1_name], param1_end_val, param2_display_name, all_params[param2_name], param2_end_val)
        
        del all_params[param1_name]
        del all_params[param2_name]
        
        for name, val in all_params.iteritems():
            if name == 'beta1':
                title_string += r'$\beta_u$:%.1f '%val
            elif name == 'beta2':
                title_string += r'$\beta_r$:%.1f '%val
            elif name == 'UFS':
                title_string += 'User:%.1f '%val
            elif name == 'RFS':
                title_string += 'Resource:%.1f '%val
            elif name == 'resource_field_increment':
                title_string += 'Resource Increment:%.1f '%val
            
        
        param1              = np.empty((len(sweep_values)))
        param2              = np.empty((len(sweep_values)))
        user_conserve       = np.empty((len(sweep_values)))
        user_neutral        = np.empty((len(sweep_values)))
        user_deplete        = np.empty((len(sweep_values)))
        resource_conserve   = np.empty((len(sweep_values)))
        resource_neutral    = np.empty((len(sweep_values)))
        resource_deplete    = np.empty((len(sweep_values)))
        parity              = np.empty((len(sweep_values)))

        #variances
        user_conserve_var       = np.empty((len(sweep_values)))
        user_neutral_var        = np.empty((len(sweep_values)))
        user_deplete_var        = np.empty((len(sweep_values)))
        resource_conserve_var   = np.empty((len(sweep_values)))
        resource_neutral_var    = np.empty((len(sweep_values)))
        resource_deplete_var    = np.empty((len(sweep_values)))
        parity_var              = np.empty((len(sweep_values)))
        
        for num, val in enumerate(sweep_values):
            #print val
            param1[num]             = val[param1_name]
            param2[num]             = val[param2_name]
            user_conserve[num]      = val["user_conserve_mean"]
            user_neutral[num]       = val["user_neutral_mean"]
            user_deplete[num]       = val["user_deplete_mean"]
            resource_conserve[num]  = val["resource_conserve_mean"]
            resource_neutral[num]   = val["resource_neutral_mean"]
            resource_deplete[num]   = val["resource_deplete_mean"]
            parity[num]             = val["parity_mean"]

            #variances
            user_conserve_var[num]      = val["user_conserve_var"]
            user_neutral_var[num]       = val["user_neutral_var"]
            user_deplete_var[num]       = val["user_deplete_var"]
            resource_conserve_var[num]  = val["resource_conserve_var"]
            resource_neutral_var[num]   = val["resource_neutral_var"]
            resource_deplete_var[num]   = val["resource_deplete_var"]
            parity_var[num]             = val["parity_var"]

            #print parity[num]
        
        #print
        #print
        if (options.print_info):
            #print info
            if options.p1 is None or options.p2 is None:
                print '!!! Both --p1 and --p2 must be set to print info !!!'
                return
            in_val_1 = float(options.p1)
            in_val_2 = float(options.p2)
            print 'Output at %s = %.3f and %s =% .3f'%(param1_name, in_val_1, param2_name, in_val_2)

            for index, val in enumerate(param1):
                #print val, param2[index], user_conserve[index]
                if val > in_val_1 and param2[index] > in_val_2:
                    val_1 = val
                    val_2 = param2[index]
                    slice_index = index
                    break
            #for index, val in enumerate(param2):
            #    if val > in_val_2:
            #        val_2 = val
            #        val_2_slice_index = index
            #        break
            #print param1.shape, param2.shape
            try:
                print '    Actual values in data %s = %.3f and %s = %.3f'%(param1_name, val_1, param2_name, val_2)
                print '        user conserve:       %.3f (+- %.6f)'%(user_conserve[slice_index], user_conserve_var[slice_index])
                print '        user neutral:        %.3f (+- %.6f)'%(user_neutral[slice_index], user_neutral_var[slice_index])
                print '        user deplete:        %.3f (+- %.6f)'%(user_deplete[slice_index], user_deplete_var[slice_index])
                print '        lattice parity:      %.3f (+- %.6f)'%(parity[slice_index], parity_var[slice_index])
                print '        resource conserve:   %.3f (+- %.6f)'%(resource_conserve[slice_index], resource_conserve_var[slice_index])
                print '        resource neutral:    %.3f (+- %.6f)'%(resource_neutral[slice_index], resource_neutral_var[slice_index])
                print '        resource deplete:    %.3f (+- %.6f)'%(resource_deplete[slice_index], resource_deplete_var[slice_index])
            except:
                print '!!! The chosen value of either --p1 or --p2 is out of the data range. !!!'
        elif (options.slice):
            #print a 1d slice through the data
            if options.p1 is None and options.p2 is None:
                print '!!! Either --p1 and --p2 must be set to print a slice !!!'
                return
            
            if options.p1 is not None and options.p2 is not None:
                print '!!! Set either --p1 or --p2, not both !!!'
                return
            
            if options.p1 == 'p2':
                diag = True
            else:
                diag = False
                if options.p1 is None:
                    for index, val in enumerate(param2):
                        #print param1[index], val, user_conserve[index]
                        if val > float(options.p2):
                            fixed_val = val
                            slice_index = index
                            break
                    print 'Fixed slice at %s = %.3f'%(param2_name, fixed_val)
                elif options.p2 is None:
                    for index, val in enumerate(param1):
                        #print val, param2[index], user_conserve[index]
                        if val > float(options.p1):
                            fixed_val = val
                            slice_index = index
                            break
                    print 'Fixed slice at %s = %.3f'%(param1_name, fixed_val)
                            
            
            print param1_name, param2_name, 'user_conserve user_conserve_var user_neutral user_neutral_var user_deplete user_deplete_var parity parity_var resource_conserve resource_conserve_var resource_neutral resource_neutral_var resource_deplete resource_deplete_var'
            for index, val in enumerate(param1):
                if diag:
                    if val == param2[index]:
                        print val, param2[index], user_conserve[index], user_conserve_var[index], user_neutral[index], user_neutral_var[index], user_deplete[index], user_deplete_var[index], parity[index], parity_var[index], resource_conserve[index], resource_conserve_var[index], resource_neutral[index], resource_neutral_var[index], resource_deplete[index], resource_deplete_var[index]
                else:
                    if options.p1 is None:
                        if param2[index] == fixed_val:
                            print val, param2[index], user_conserve[index], user_conserve_var[index], user_neutral[index], user_neutral_var[index], user_deplete[index], user_deplete_var[index], parity[index], parity_var[index], resource_conserve[index], resource_conserve_var[index], resource_neutral[index], resource_neutral_var[index], resource_deplete[index], resource_deplete_var[index]
                    elif options.p2 is None:
                        if val == fixed_val:
                            print val, param2[index], user_conserve[index], user_conserve_var[index], user_neutral[index], user_neutral_var[index], user_deplete[index], user_deplete_var[index], parity[index], parity_var[index], resource_conserve[index], resource_conserve_var[index], resource_neutral[index], resource_neutral_var[index], resource_deplete[index], resource_deplete_var[index]
                        
                    

        else:
            #these are plotting routines
            in_slice_val = 3.0
            slice_axis = 'x'
            if (options.p1 is not None and options.p2 is not None):
                in_slice_val = float(options.p2)
            elif (options.p1 is not None and options.p2 is None):
                in_slice_val = float(options.p1)
                slice_axis = 'y'
            elif (options.p1 is None and options.p2 is not None):
                in_slice_val = float(options.p2)

            xi = np.linspace(min(param1),max(param1))
            yi = np.linspace(min(param2),max(param2))
            
            X, Y = np.meshgrid(xi, yi)
            
            Z_user_conserve = griddata(param1, param2, user_conserve, xi, yi)
            Z_user_neutral = griddata(param1, param2, user_neutral, xi, yi)
            Z_user_deplete = griddata(param1, param2, user_deplete, xi, yi)
            Z_resource_conserve = griddata(param1, param2, resource_conserve, xi, yi)
            Z_resource_neutral = griddata(param1, param2, resource_neutral, xi, yi)
            Z_resource_deplete = griddata(param1, param2, resource_deplete, xi, yi)
            Z_parity = griddata(param1, param2, parity, xi, yi)
            
            #print len(parity), len(resource_deplete)
            
            cmap   = plt.get_cmap('Blues_r')
            dt = np.dtype((np.float, (4)))
            Z_parity_colors = np.empty(Z_parity.shape, dtype=dt)

            it = np.nditer(Z_parity, flags=['multi_index'])
            while not it.finished:
                #print Z_parity[it.multi_index]
                Z_parity_colors[it.multi_index] = cmap(Z_parity[it.multi_index])
                #print colors[it.multi_index], C[it.multi_index], it.multi_index
                it.iternext()
        
            # set up plot
            font_family = 'Arial'
            thD_tick_fontsize = 6
            thD_axis_labels_fontsize = 6
            thD_frame_linewidth = 0.25
            thD_label_fontsize = 9
            twD_tick_fontsize = 6
            twD_axis_labels_fontsize = 6
            twD_title_fontsize = 7
            thD_cb_fontsize = 5
            legend_fontsize = 5
            
            plot_width = 6.3109
            plot_height = 6.3109

            frame_linewidth = 0.25
            
            thD_plot_width_frac = 0.31
            thD_plot_height_frac =  0.31
            
            thD_plot_user_bottom_frac = 0.7
            if float(options.view_angle) < 80 or (float(options.view_angle) > 180 and float(options.view_angle) < 270):
                thD_plot_user_left_frac = 0.04
            else:
                thD_plot_user_left_frac = 0.01
            
            thD_plot_sep_frac = 0.01
            
            thD_plot_resource_bottom_frac = thD_plot_user_bottom_frac - thD_plot_height_frac - thD_plot_sep_frac
            if float(options.view_angle) < 80 or (float(options.view_angle) > 180 and float(options.view_angle) < 270):
                thD_plot_resource_left_frac = 0.04
            else:
                thD_plot_resource_left_frac = 0.01
            
            cb_width_frac = 0.6
            cb_height_frac = 0.01
            cb_left_frac = 0.05
            cb_bottom_frac = thD_plot_user_bottom_frac - thD_plot_height_frac - thD_plot_sep_frac - 0.04
            
            thD_legend_left_frac = 0.96 
            thD_legend_bottom_frac = cb_bottom_frac + 0.04
            
            #[0.05, 0.05, 0.9, 0.3]
            twD_plot_width_frac = 0.9
            twD_plot_height_frac = 0.14
            twD_plot_left_frac = 0.07
            twD_plot_bottom_frac = 0.13
            
            thD_plot_slice_linewidth = 2
            thD_plot_slice_linecolor = 'k'
            thD_plot_linestyle = ':'
            twD_plot_linewidth = 2
            twD_plot_linecolor = ([ '#18B3C0',
                                    '#6A90D1',
                                    '#3B3BC0',
                                    'k'])
            twD_plot_linestyle = [':','--']

            #print Z_user_neutral.max(), Z_user_neutral[np.where(Z_user_neutral == Z_user_neutral.max())], X[np.where(Z_user_neutral == Z_user_neutral.max())], Y[np.where(Z_user_neutral == Z_user_neutral.max())]

            #matplotlib.rc('axes', linewidth=0.25)
            
            fig = plt.figure(figsize=[6.3109, 6.3109],dpi=100)
            
            user_conserve_ax = fig.add_axes([thD_plot_user_left_frac, thD_plot_user_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            user_conserve_plot = user_conserve_ax.plot_surface(X, Y, Z_user_conserve, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            user_conserve_plot.set_edgecolors('k')
            #user_conserve_ax.plot_surface(X, Y, Z_user_conserve, rstride=3, cstride=3, cmap=cm.jet, linewidth=0.25, norm=matplotlib.colors.Normalize(vmin=0, vmax=1))
            user_conserve_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_conserve_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_conserve_ax.set_zlabel("conserve fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in user_conserve_ax.xaxis.get_major_ticks() + user_conserve_ax.yaxis.get_major_ticks() + user_conserve_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            user_conserve_ax.view_init(azim=float(options.view_angle))

            user_neutral_ax = fig.add_axes([thD_plot_user_left_frac + thD_plot_width_frac + thD_plot_sep_frac, thD_plot_user_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            user_neutral_plot = user_neutral_ax.plot_surface(X, Y, Z_user_neutral, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            user_neutral_plot.set_edgecolors('k')
            user_neutral_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_neutral_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_neutral_ax.set_zlabel("neutral fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in user_neutral_ax.xaxis.get_major_ticks() + user_neutral_ax.yaxis.get_major_ticks() + user_neutral_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            user_neutral_ax.view_init(azim=float(options.view_angle))
            
            user_deplete_ax = fig.add_axes([thD_plot_user_left_frac + 2.0*thD_plot_width_frac + 2.0*thD_plot_sep_frac, thD_plot_user_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            user_deplete_plot = user_deplete_ax.plot_surface(X, Y, Z_user_deplete, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            user_deplete_plot.set_edgecolors('k')
            user_deplete_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_deplete_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            user_deplete_ax.set_zlabel("deplete fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in user_deplete_ax.xaxis.get_major_ticks() + user_deplete_ax.yaxis.get_major_ticks() + user_deplete_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            user_deplete_ax.view_init(azim=float(options.view_angle))
            
            
            resource_conserve_ax = fig.add_axes([thD_plot_resource_left_frac, thD_plot_resource_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            resource_conserve_plot = resource_conserve_ax.plot_surface(X, Y, Z_resource_conserve, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            resource_conserve_plot.set_edgecolors('k')
            resource_conserve_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_conserve_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_conserve_ax.set_zlabel("conserve fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in resource_conserve_ax.xaxis.get_major_ticks() + resource_conserve_ax.yaxis.get_major_ticks() + resource_conserve_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            resource_conserve_ax.view_init(azim=float(options.view_angle))

            resource_neutral_ax = fig.add_axes([thD_plot_resource_left_frac + thD_plot_width_frac + thD_plot_sep_frac, thD_plot_resource_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            resource_neutral_plot = resource_neutral_ax.plot_surface(X, Y, Z_resource_neutral, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            resource_neutral_plot.set_edgecolors('k')
            resource_neutral_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_neutral_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_neutral_ax.set_zlabel("neutral fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in resource_neutral_ax.xaxis.get_major_ticks() + resource_neutral_ax.yaxis.get_major_ticks() + resource_neutral_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            resource_neutral_ax.view_init(azim=float(options.view_angle))
            
            resource_deplete_ax = fig.add_axes([thD_plot_resource_left_frac + 2.0*thD_plot_width_frac + 2.0*thD_plot_sep_frac, thD_plot_resource_bottom_frac, thD_plot_width_frac, thD_plot_height_frac],projection='3d')
            resource_deplete_plot = resource_deplete_ax.plot_surface(X, Y, Z_resource_deplete, shade=False, rstride=3, cstride=3, linewidths=0.25, facecolors=Z_parity_colors)
            resource_deplete_plot.set_edgecolors('k')
            resource_deplete_ax.set_xlabel(param1_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_deplete_ax.set_ylabel(param2_display_name, family=font_family, size=thD_axis_labels_fontsize)
            resource_deplete_ax.set_zlabel("deplete fraction", family=font_family, size=thD_axis_labels_fontsize)
            for tick in resource_deplete_ax.xaxis.get_major_ticks() + resource_deplete_ax.yaxis.get_major_ticks() + resource_deplete_ax.zaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(thD_tick_fontsize)
                tick.tick1line.set_lw(thD_frame_linewidth)
            resource_deplete_ax.view_init(azim=float(options.view_angle))
            
            plt.figtext(0.01,thD_plot_user_bottom_frac+0.6*thD_plot_height_frac,'User',rotation=90,family=font_family,size=thD_label_fontsize)
            plt.figtext(0.01,thD_plot_resource_bottom_frac+0.6*thD_plot_height_frac,'Resource',rotation=90,family=font_family,size=thD_label_fontsize)
            
            #plot the color bar
            cb_ax = fig.add_axes([cb_left_frac, cb_bottom_frac, cb_width_frac, cb_height_frac])
            norm = matplotlib.colors.Normalize(vmin=0, vmax=1)
            cb = matplotlib.colorbar.ColorbarBase(cb_ax, cmap=cm.Blues_r,
                       norm=norm,
                       orientation='horizontal')
            cb.set_ticks(np.arange(0,1.1,0.1))
            #for tick in cb.get_major_ticks():
            #    tick.label1.set_fontname(font_family)
            #    tick.label1.set_fontsize(twD_tick_fontsize)
            #    tick.tick1line.set_lw(thD_frame_linewidth)
            #    tick.tick1line.set_alpha(0)
            for label in cb_ax.xaxis.get_ticklabels():
                label.set_fontname(font_family)
                label.set_fontsize(thD_cb_fontsize)
            for line in cb_ax.xaxis.get_ticklines():
                line.set_alpha(0)
            
            cb_ax.set_title('Lattice parity', family=font_family, size=legend_fontsize, ha='left', position=(0,1) )
            
            #plot 2d view
            twD_ax = fig.add_axes([twD_plot_left_frac, twD_plot_bottom_frac, twD_plot_width_frac, twD_plot_height_frac])
            
            slice_val = None
            slice_index = None
            
            if (slice_axis == 'x'):
                #plot x vs z
                frozen_param = param2_name
                frozen_param_name = param2_display_name
                #first find the index that is closest to in_slice_val
                for index, val in enumerate(yi):
                    if val > in_slice_val:
                        slice_val = val
                        slice_index = index
                        break
                twD_ax.plot(xi,Z_user_conserve[slice_index,::],    c=twD_plot_linecolor[0],lw=twD_plot_linewidth)
                twD_ax.plot(xi,Z_user_neutral[slice_index,::],     c=twD_plot_linecolor[1],lw=twD_plot_linewidth)
                twD_ax.plot(xi,Z_user_deplete[slice_index,::],     c=twD_plot_linecolor[2],lw=twD_plot_linewidth)
                twD_ax.plot(xi,Z_resource_conserve[slice_index,::],c=twD_plot_linecolor[0],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(xi,Z_resource_neutral[slice_index,::], c=twD_plot_linecolor[1],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(xi,Z_resource_deplete[slice_index,::], c=twD_plot_linecolor[2],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(xi,Z_parity[slice_index,::], c=twD_plot_linecolor[3],lw=twD_plot_linewidth,ls=twD_plot_linestyle[1])
                
                twD_ax.set_xlabel(param1_display_name, family=font_family, size=twD_axis_labels_fontsize, ha='left', position=(0,1) )
                
                twD_ax.set_xlim(right=xi.max())

                if not options.clean_3d:
                    user_conserve_ax.plot(xi, slice_val*np.ones(len(xi)), Z_user_conserve[slice_index,::], thD_plot_linestyle, lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Conserve fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    user_neutral_ax.plot(xi, slice_val*np.ones(len(xi)), Z_user_neutral[slice_index,::], thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Neutral fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    user_deplete_ax.plot(xi, slice_val*np.ones(len(xi)), Z_user_deplete[slice_index,::], thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Deplete fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    
                    resource_conserve_ax.plot(xi, slice_val*np.ones(len(xi)), Z_resource_conserve[slice_index,::], thD_plot_linestyle, lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Conserve fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    resource_neutral_ax.plot(xi, slice_val*np.ones(len(xi)), Z_resource_neutral[slice_index,::], thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Neutral fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    resource_deplete_ax.plot(xi, slice_val*np.ones(len(xi)), Z_resource_deplete[slice_index,::], thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Deplete fraction at %s = %.2f'%(frozen_param_name, slice_val))

            elif (slice_axis == 'y'):
                #plot y vs z
                frozen_param = param1_name
                frozen_param_name = param1_display_name
                for index, val in enumerate(xi):
                    if val > in_slice_val:
                        slice_val = val
                        slice_index = index
                        break
                twD_ax.plot(yi,Z_user_conserve[::,slice_index],    c=twD_plot_linecolor[0],lw=twD_plot_linewidth)
                twD_ax.plot(yi,Z_user_neutral[::,slice_index],     c=twD_plot_linecolor[1],lw=twD_plot_linewidth)
                twD_ax.plot(yi,Z_user_deplete[::,slice_index],     c=twD_plot_linecolor[2],lw=twD_plot_linewidth)
                twD_ax.plot(yi,Z_resource_conserve[::,slice_index],c=twD_plot_linecolor[0],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(yi,Z_resource_neutral[::,slice_index], c=twD_plot_linecolor[1],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(yi,Z_resource_deplete[::,slice_index], c=twD_plot_linecolor[2],lw=twD_plot_linewidth,ls=twD_plot_linestyle[0])
                twD_ax.plot(yi,Z_parity[slice_index,::], c=twD_plot_linecolor[3],lw=twD_plot_linewidth,ls=twD_plot_linestyle[1])
                
                twD_ax.set_xlabel(param2_display_name, family=font_family, size=twD_axis_labels_fontsize, ha='left', position=(0,1) )
                
                twD_ax.set_xlim(right=yi.max())
                
                if not options.clean_3d:
                    user_conserve_ax.plot(slice_val*np.ones(len(yi)),yi,Z_user_conserve[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Conserve fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    user_neutral_ax.plot(slice_val*np.ones(len(yi)),yi,Z_user_neutral[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Neutral fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    user_deplete_ax.plot(slice_val*np.ones(len(yi)),yi,Z_user_deplete[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Deplete fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    
                    resource_conserve_ax.plot(slice_val*np.ones(len(yi)),yi,Z_resource_conserve[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Conserve fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    resource_neutral_ax.plot(slice_val*np.ones(len(yi)),yi,Z_resource_neutral[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Neutral fraction at %s = %.2f'%(frozen_param_name, slice_val))
                    resource_deplete_ax.plot(slice_val*np.ones(len(yi)),yi,Z_resource_deplete[::,slice_index],thD_plot_linestyle,lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor, label='Deplete fraction at %s = %.2f'%(frozen_param_name, slice_val))

            twD_ax.set_ylabel("state fraction",family=font_family, size=twD_axis_labels_fontsize)
            for tick in twD_ax.xaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(twD_tick_fontsize)
            for tick in twD_ax.yaxis.get_major_ticks():
                tick.label1.set_fontname(font_family)
                tick.label1.set_fontsize(twD_tick_fontsize)
            twD_ax.set_title('State fraction at %s = %.2f'%(frozen_param_name, slice_val), family=font_family, size=twD_title_fontsize, ha='left', position=(0,1) )
            
            #create the legends
            leg_borderpad = 0.3
            leg_labelspacing = 0
            leg_handlelength = 2
            leg_handletextpad = 0
            leg_borderaxespad = 0
            leg_columnspacing = 1
            leg_markerscale = 3

            if not options.clean_3d:
                proxy_ax = fig.add_axes([0,0,0,0])
                proxy = proxy_ax.plot((0,1), (0,1), '--', lw=thD_plot_slice_linewidth, c=thD_plot_slice_linecolor)
                thD_legend = fig.legend(proxy, ['2D slice at %s = %.2f plotted below'%(frozen_param_name, slice_val)], bbox_to_anchor=(thD_legend_left_frac, thD_legend_bottom_frac), borderpad=leg_borderpad, handlelength=leg_handlelength, handletextpad=leg_handletextpad)

            twD_legend = twD_ax.legend(('User conserve fraction', 'User neutral fraction', 'User deplete fraction','Resource conserve fraction', 'Resource neutral fraction', 'Resource deplete fraction', 'Lattice parity'), loc=(0.115,-0.85), ncol=4, borderpad=leg_borderpad, handlelength=leg_handlelength, handletextpad=leg_handletextpad,columnspacing=leg_columnspacing)

            if not options.clean_3d:
                for label in twD_ax.get_legend().get_texts() + thD_legend.get_texts():
                    label.set_fontname(font_family)
                    label.set_fontsize(legend_fontsize)
            else:
                for label in twD_ax.get_legend().get_texts():
                    label.set_fontname(font_family)
                    label.set_fontsize(legend_fontsize)


            #take care of all of the frame line widths
            for spine in twD_ax.spines.itervalues():
                spine.set_lw(frame_linewidth)
            
            cb.outline.set_lw(frame_linewidth)
            
            user_conserve_ax.xaxis.line.set_lw(frame_linewidth)
            user_conserve_ax.yaxis.line.set_lw(frame_linewidth)
            user_conserve_ax.zaxis.line.set_lw(frame_linewidth)
            user_neutral_ax.xaxis.line.set_lw(frame_linewidth)
            user_neutral_ax.yaxis.line.set_lw(frame_linewidth)
            user_neutral_ax.zaxis.line.set_lw(frame_linewidth)
            user_deplete_ax.xaxis.line.set_lw(frame_linewidth)
            user_deplete_ax.yaxis.line.set_lw(frame_linewidth)
            user_deplete_ax.zaxis.line.set_lw(frame_linewidth)
            
            resource_conserve_ax.xaxis.line.set_lw(frame_linewidth)
            resource_conserve_ax.yaxis.line.set_lw(frame_linewidth)
            resource_conserve_ax.zaxis.line.set_lw(frame_linewidth)
            resource_neutral_ax.xaxis.line.set_lw(frame_linewidth)
            resource_neutral_ax.yaxis.line.set_lw(frame_linewidth)
            resource_neutral_ax.zaxis.line.set_lw(frame_linewidth)
            resource_deplete_ax.xaxis.line.set_lw(frame_linewidth)
            resource_deplete_ax.yaxis.line.set_lw(frame_linewidth)
            resource_deplete_ax.zaxis.line.set_lw(frame_linewidth)
            
            if not options.clean_3d:
                thD_legend.get_frame().set_linewidth(frame_linewidth)
                twD_legend.get_frame().set_linewidth(frame_linewidth)
            
            
            #for spine in cb_ax.spines.itervalues():
            #    print spine
            
            #frame = twD_plot.get_frame() 
            #frame.set_linewidth(0.25)
            #ax.patch.set_linewidth(4.0)

            
                    
            '''
            for label in user_conserve_ax.get_legend().get_texts():
                label.set_fontname(font_family)
                label.set_fontsize(5)
            user_conserve_ax.get_legend().get_frame().set_linewidth(0.25)
            for label in user_neutral_ax.get_legend().get_texts():
                label.set_fontname(font_family)
                label.set_fontsize(5)
            user_neutral_ax.get_legend().get_frame().set_linewidth(0.25)
            for label in user_deplete_ax.get_legend().get_texts():
                label.set_fontname(font_family)
                label.set_fontsize(5)
            user_deplete_ax.get_legend().get_frame().set_linewidth(0.25)
            for label in ax5.get_legend().get_texts():
                label.set_fontname(font_family)
                label.set_fontsize(5)
            ax5.get_legend().get_frame().set_linewidth(0.25)
            '''
            
            #fig.text(0.02, 0.98, title_string, fontsize=20, ha='left', va='top', alpha=1.0, family=font_family)
                   
            plt.savefig("%s_%s_%.2f.pdf"%(sys_name,frozen_param, slice_val), format="pdf")

if __name__ == "__main__": 
    sys.exit(main())