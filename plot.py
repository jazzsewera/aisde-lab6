import csv
import sys
import getopt
import numpy as np
from datetime import datetime
try:
    from matplotlib import pyplot as plt
except Exception:
    import matplotlib
    matplotlib.use('pdf')
    from matplotlib import pyplot as plt


def save_plot(ax, name='plot', const_temp=False):
    if not const_temp:
        ax.set_xscale('log')
        plt.xlabel('Temperature')
    else:
        plt.xlabel('Algorithm pass no.')
    dt = datetime.now().strftime('%H-%M-%S')
    plt.ylabel('Cost')
    plt.savefig('out/{0}_{1}.pdf'.format(name, dt))


def plot_simul_annealing(const_temp=False):
    temp_array = []
    pos_array = []
    with open('cooling.csv', 'r', newline='') as cooling_file:
        reader = csv.reader(cooling_file, delimiter=';')
        for row in reader:
            temp_array.append(float(row[0]))
            pos_array.append(int(row[1]))

    ax = plt.axes()

    if const_temp:
        ax.plot(np.arange(len(pos_array)), pos_array, ',')
    else:
        ax.plot(temp_array, pos_array, ',')
        ax.invert_xaxis()

    save_plot(ax, 'temp_pos', const_temp)


def main(argv):
    const_temp = False
    try:
        opts, args = getopt.getopt(argv, 't')
    except getopt.GetoptError:
        exit()
    for opt, arg in opts:
        if opt == '-t':
            const_temp = True

    plot_simul_annealing(const_temp)


if __name__ == '__main__':
    main(sys.argv[1:])
