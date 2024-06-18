import pandas as pd
import matplotlib.pyplot as plt
import glob

def plot_gflops():
    csv_files = sorted(glob.glob("output_ge*.csv")) 
    
    plt.figure(figsize=(12, 8))
    
    for file in csv_files:
        data = pd.read_csv(file)
        label = file.split('_')[1].split('.')[0] 
        plt.plot(data['Matrix Size'], data['GFLOPS'], marker='o', label=label)
    
    plt.xlabel('Matrix Size')
    plt.ylabel('GFLOPS')
    plt.title('GFLOPS for different matrix sizes and implementations')
    plt.legend()
    plt.grid(True)
    plt.savefig('gflops_plot.png')
    plt.show()

if __name__ == "__main__":
    plot_gflops()
