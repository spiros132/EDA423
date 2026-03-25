minimum = -10
maximum = 14

freq_indices = []

for index in range(minimum, maximum+1):
    freq = 440 * pow(2, index/12)
    period = 1/(2*freq) * pow(10,6)
    freq_indices.append(round(period))

print(freq_indices)