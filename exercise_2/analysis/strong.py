import argparse

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def import_data(path):
    return pd.read_csv(path, sep='\t', header=None)

def plot_speedup(ax, N, X, ERR, size, lib, places_policy):
    count = 0
    for n, t, err in zip(N, X, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', markersize=7.5, linewidth=1.5, elinewidth=2, capsize=0,
                    label='{} ({})'.format(lib, places_policy))
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(45)

def plot_comparison(ax, N, X, lib, places_policy):
    count = 0
    for n, t in zip(N, X):
        # ax.plot(N, t)
        ax.plot(n, t, marker='s', markersize=7.5, linewidth=1.5,
                    label='{} ({})'.format(lib, places_policy))
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(45)
    ax.legend()

def get_speedup_stats(punits, data):
    t = data.iloc[:, -2].values
    t1 = t[np.where(data.iloc[:, 0].values == 1)[0]]
    t1_mean = np.mean(t1)
    t1_std = np.std(t1)

    s_mean = np.empty(len(punits))
    s_std = np.empty(len(punits))
    for n in range(len(punits)):
        t_n = t[np.where(data.iloc[:, 0].values == punits[n])[0]]
        t_mean = np.mean(t_n)
        t_std = np.std(t_n)

        s_mean[n] = t1_mean / t_mean
        s_std[n] = s_mean[n] * (t1_std / t1_mean + t_std / t_mean)

    return s_mean, s_std

def get_gflops_stats(size, data):
    gflops = data.iloc[:, -1].values
    print(gflops)
    g_mean = np.empty(len(size))
    g_std = np.empty(len(size))
    for n in range(len(size)):
        g_n = gflops[np.where(data.iloc[:, 0].values == size[n])[0]]
        g_mean_n = np.mean(g_n)
        g_std_n = np.std(g_n)

        g_mean[n] = g_mean_n
        g_std[n] = g_std_n

    return g_mean, g_std

def main():
    parser = argparse.ArgumentParser(description='Parse args for plotting time data')
    parser.add_argument('--prec', type=str, help='precision')
    args = parser.parse_args()

    # places = ['s', 'c', 't']
    # pbindings = ['s', 'c']
    # pols = ['ss',
    #         'sc',
    #         'cs',
    #         'cc',
    #         'ts',
    #         'tc']
    # places_binding_dict = {'ss': 'socket/spread',
    #                        'sc': 'sockets/close',
    #                        'cs': 'cores/spread',
    #                        'cc': 'cores/close',
    #                        'ts': 'threads/spread',
    #                        'tc': 'threads/close'}
    places = [
        # 's',
        'c',
        # 't'
    ]
    pbindings = ['ss', 'cs']
    pols = ['sss',
            'scs',
            'css',
            'ccs',
            'tss',
            'tcs']
    places_binding_dict = {
       #  'ss': 'socket/spread',
       # 'sc': 'sockets/close',
       'css': 'cores/spread',
       'ccs': 'cores/close',
       # 'ts': 'threads/spread',
       # 'tc': 'threads/close'
    }
    fig1, ax1 = plt.subplots(1, 1, figsize=(9, 5))
    fig2, ax2 = plt.subplots(1, 1, figsize=(9, 5))
    i = 0

    # TPP_c = 2662.4/64
    TPP_c = 83.2
    if args.prec == 'single': TPP_c *= 2
    for place in places:
        for pb in pbindings:
            places_binding = places_binding_dict[place + pb]
            mkl_path = "./strong/{}/mkl_".format(args.prec) + place + pb + ".txt"
            oblas_path = "./strong/{}/oblas_".format(args.prec) + place + pb + ".txt"
            mkl_all_data = import_data(mkl_path)
            mkl_groups = mkl_all_data.groupby([mkl_all_data.columns[i] for i in range(1, 4)])
            print('mkl\n', mkl_all_data)
            oblas_all_data = import_data(oblas_path)
            oblas_groups = oblas_all_data.groupby([oblas_all_data.columns[i] for i in range(1, 4)])
            print('oblas\n', oblas_all_data)
            N, S, ERR_S, size = {'mkl': [], 'oblas': []}, \
                                {'mkl': [], 'oblas': []}, \
                                {'mkl': [], 'oblas': []}, \
                                {'mkl': [], 'oblas': []}
            REL = []
            for oblas_key, mkl_key in zip(list(oblas_groups.groups.keys()), list(mkl_groups.groups.keys())):
                mkl_data = mkl_groups.get_group(mkl_key)
                oblas_data = oblas_groups.get_group(oblas_key)

                assert(all(np.unique(mkl_data.iloc[:, 0].values) == np.unique(oblas_data.iloc[:, 0].values)))
                punits = np.unique(mkl_data.iloc[:, 0].values)

                s_mean_mkl, s_std_mkl = get_gflops_stats(punits, mkl_data)
                N['mkl'] += [punits]
                S['mkl'] += [s_mean_mkl]
                ERR_S['mkl'] += [s_std_mkl]
                size['mkl'] += [mkl_key]

                s_mean_oblas, s_std_oblas = get_gflops_stats(punits, oblas_data)
                N['oblas'] += [punits]
                S['oblas'] += [s_mean_oblas]
                ERR_S['oblas'] += [s_std_oblas]
                size['oblas'] += [oblas_key]

                REL += [(s_mean_mkl - s_mean_oblas) / s_mean_oblas]

            # plot_speedup(ax1[i], N['mkl'], S['mkl'], ERR_S['mkl'], size['mkl'], 'mkl', places_binding)
            # plot_speedup(ax1[i], N['oblas'], S['oblas'], ERR_S['oblas'], size['oblas'], 'oblas', places_binding)
            #
            # plot_comparison(ax2[i], N['mkl'], REL, 'mkl-oblas', places_binding)

            plot_speedup(ax1, N['mkl'], S['mkl'], ERR_S['mkl'], size['mkl'], 'mkl', places_binding)
            plot_speedup(ax1, N['oblas'], S['oblas'], ERR_S['oblas'], size['oblas'], 'oblas', places_binding)

            plot_comparison(ax2, N['mkl'], REL, 'mkl-oblas', places_binding)

        # ax1[i].plot([i for i in range(1, np.max([N[key] for key in N.keys()]) + 1)],
        #            [i for i in range(1, np.max([N[key] for key in N.keys()]) + 1)],
        #            color='red', label='linear', linestyle='dashed')
        # ax1[i].plot([i for i in range(1, 64)], [TPP_c * i for i in range(1, 64)], linestyle='--', label='TPP')
        # ax1[i].legend()

        ax1.plot([i for i in range(1, 13)], [TPP_c * i for i in range(1, 13)], linestyle='--', label='TPP')
        ax1.legend()

        i += 1
    fig1.text(0.5, 1 - 0.08, 'cores', ha='center', fontsize=14)
    fig1.text(0.04, 0.5, 'GFLOPS', va='center', rotation='vertical', fontsize=14)
    fig2.text(0.5, 1 - 0.08, 'cores', ha='center', fontsize=14)
    fig2.text(0.04, 0.5, 'relative change', va='center', rotation='vertical', fontsize=14)
    plt.xticks()
    plt.yticks()
    plt.show()
    plt.show()



if __name__ == '__main__':
    main()