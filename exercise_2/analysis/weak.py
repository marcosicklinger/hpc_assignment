import argparse

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def import_data(path):
    return pd.read_csv(path, sep='\t', header=None)

def plot_gflops(ax, N, X, ERR, cores, lib, places_policy):
    count = 0
    for n, t, err in zip(N, X, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', markersize=7.5, linewidth=1.5, elinewidth=2, capsize=0,
                    label='{} ({})'.format(lib, places_policy))
        count += 1
    for tick in ax.get_xticklabels():
        tick.set_rotation(45)
    ax.legend()

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

def get_gflops_stats(size, data):
    gflops = data.iloc[:, -1].values

    g_mean = np.empty(len(size))
    g_std = np.empty(len(size))
    for n in range(len(size)):
        g_n = gflops[np.where(data.iloc[:, 1].values == size[n])[0]]
        g_mean_n = np.mean(g_n)
        g_std_n = np.std(g_n)

        g_mean[n] = g_mean_n
        g_std[n] = g_std_n

    return g_mean, g_std

def main():
    parser = argparse.ArgumentParser(description='Parse args for plotting time data')
    parser.add_argument('--prec', type=str, help='precision')
    # parser.add_argument('--mkl_path', type=str, help='path to txt file')
    # parser.add_argument('--oblas_path', type=str, help='path to txt file')
    # parser.add_argument('--plbind', type=str, help='places\policy')
    args = parser.parse_args()

    TPP = 0
    if args.prec == 'single':
        TPP = 5324.8
    if args.prec == 'double':
        TPP = 2662.4

    places = ['s', 'c', 't']
    pbindings = ['s', 'c']
    pols = ['ss', 'sc', 'cs', 'cc', 'ts', 'tc']
    places_binding_dict = {'ss': 'socket/spread',
                           'sc': 'sockets/close',
                           'cs': 'cores/spread',
                           'cc': 'cores/close',
                           'ts': 'threads/spread',
                           'tc': 'threads/close'}

    fig1, ax1 = plt.subplots(1, 3, figsize=(15, 4.5))
    fig2, ax2 = plt.subplots(1, 3, figsize=(15, 4.5))
    i = 0
    for place in places:
        for pb in pbindings:
            places_binding = places_binding_dict[place + pb]
            mkl_path = "./weak/{}/mkl_".format(args.prec) + place + pb + ".txt"
            oblas_path = "./weak/{}/oblas_".format(args.prec) + place + pb + ".txt"
            mkl_all_data = import_data(mkl_path)
            mkl_groups = mkl_all_data.groupby([mkl_all_data.columns[0]])
            oblas_all_data = import_data(oblas_path)
            oblas_groups = oblas_all_data.groupby([oblas_all_data.columns[0]])
            N, G, ERR_G, cores = {'mkl': [], 'oblas': []}, \
                                  {'mkl': [], 'oblas': []}, \
                                  {'mkl': [], 'oblas': []}, \
                                  {'mkl': [], 'oblas': []}
            REL = []
            for oblas_key, mkl_key in zip(list(oblas_groups.groups.keys()), list(mkl_groups.groups.keys())):
                mkl_data = mkl_groups.get_group(mkl_key)
                print('mkl grp\n', mkl_data)
                oblas_data = oblas_groups.get_group(oblas_key)
                print('oblas grp\n', oblas_data)

                assert all(np.unique(mkl_data.iloc[:, 1].values) == np.unique(oblas_data.iloc[:, 1].values))
                assert all(np.unique(mkl_data.iloc[:, 2].values) == np.unique(oblas_data.iloc[:, 2].values))
                assert all(np.unique(mkl_data.iloc[:, 3].values) == np.unique(oblas_data.iloc[:, 3].values))
                mnk = np.unique(mkl_data.iloc[:, 1].values)

                s_mean_mkl, s_std_mkl = get_gflops_stats(mnk, mkl_data)
                N['mkl'] += [mnk]
                G['mkl'] += [s_mean_mkl]
                ERR_G['mkl'] += [s_std_mkl]
                cores['mkl'] += [mkl_key]

                s_mean_oblas, s_std_oblas = get_gflops_stats(mnk, oblas_data)
                N['oblas'] += [mnk]
                G['oblas'] += [s_mean_oblas]
                ERR_G['oblas'] += [s_std_oblas]
                cores['oblas'] += [oblas_key]

                REL += [(s_mean_mkl - s_mean_oblas)/s_mean_oblas]

            plot_gflops(ax1[i], N['mkl'], G['mkl'], ERR_G['mkl'], cores['mkl'], 'mkl', places_binding)
            plot_gflops(ax1[i], N['oblas'], G['oblas'], ERR_G['oblas'], cores['oblas'], 'oblas', places_binding)

            plot_comparison(ax2[i], N['mkl'], REL, 'mkl-oblas', places_binding)
        i += 1

    fig1.text(0.5, 1-0.08, 'size', ha='center', fontsize=14)
    fig1.text(0.04, 0.5, 'GFLOPS', va='center', rotation='vertical', fontsize=14)
    fig2.text(0.5, 1 - 0.08, 'size', ha='center', fontsize=14)
    fig2.text(0.04, 0.5, 'relative change', va='center', rotation='vertical', fontsize=14)
    plt.xticks()
    plt.yticks()
    plt.show()



if __name__ == '__main__':
    main()