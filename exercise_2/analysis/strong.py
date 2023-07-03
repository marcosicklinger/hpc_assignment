import argparse

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def import_data(path):
    return pd.read_csv(path, sep='\t', header=None)

def plot_speedup(ax, N, X, ERR, size, lib, policy):
    count = 0
    for n, t, err in zip(N, X, ERR):
        # ax.plot(N, t)
        ax.errorbar(n, t, err, marker='s', linewidth=0.5, elinewidth=2, capsize=0,
                    label='{}x{}'.format(size[count][0], size[count][1]))
        count += 1
    ax.plot([i for i in range(1, np.max(N) + 1)], [i for i in range(1, np.max(N) + 1)], color='red', label='ideal')
    xlabel = "cores"
    ax.set_xlabel(xlabel)
    ax.set_ylabel('speedup')
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

def main():
    parser = argparse.ArgumentParser(description='Parse args for plotting time data')
    parser.add_argument('--mkl_path', type=str, help='path to txt file')
    parser.add_argument('--oblas_path', type=str, help='path to txt file')
    args = parser.parse_args()

    mkl_all_data = import_data(args.mkl_path)
    mkl_groups = mkl_all_data.groupby([mkl_all_data.columns[i] for i in range(1, 4)])
    print('mkl\n', mkl_all_data)
    oblas_all_data = import_data(args.oblas_path)
    oblas_groups = oblas_all_data.groupby([oblas_all_data.columns[i] for i in range(1, 4)])
    print('oblas\n', oblas_all_data)
    N, S, ERR_S, size = {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}, \
                        {'mkl': [], 'oblas': []}

    for oblas_key, mkl_key in zip(list(oblas_groups.groups.keys()), list(mkl_groups.groups.keys())):
        mkl_data = mkl_groups.get_group(mkl_key)
        oblas_data = oblas_groups.get_group(oblas_key)

        assert(all(np.unique(mkl_data.iloc[:, 0].values) == np.unique(oblas_data.iloc[:, 0].values)))
        punits = np.unique(mkl_data.iloc[:, 0].values)

        s_mean_mkl, s_std_mkl = get_speedup_stats(punits, mkl_data)
        N['mkl'] += [punits]
        S['mkl'] += [s_mean_mkl]
        ERR_S['mkl'] += [s_std_mkl]
        size['mkl'] += [mkl_key]

        s_mean_oblas, s_std_oblas = get_speedup_stats(punits, oblas_data)
        N['oblas'] += [punits]
        S['oblas'] += [s_mean_oblas]
        ERR_S['oblas'] += [s_std_oblas]
        size['oblas'] += [oblas_key]

    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    plot_speedup(ax, N['mkl'], S['mkl'], ERR_S['mkl'], size['mkl'], 'mkl', 'sockets')
    plot_speedup(ax, N['oblas'], S['oblas'], ERR_S['oblas'], size['oblas'], 'oblas', 'sockets')
    plt.show()



if __name__ == '__main__':
    main()

