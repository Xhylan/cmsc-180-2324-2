import random
import time


def track_progress(current, total):
    progress = (current / total) * 100.00
    print(f"\rProgress: {progress:.2f}% - {current} out of {total}", end="")


def measure_execution_time(function):
    def wrapper(*args, **kwargs):
        print(f"Starting execution of {function.__name__}...")
        start = time.time()
        result = function(*args, **kwargs)
        end = time.time()
        elapsed_time = end - start
        print(
            f"\rFunction {function.__name__} ran for {elapsed_time:.4f} seconds.")
        return result
    return wrapper


@measure_execution_time
def init_matrix(n, min, max):
    matrix = []

    for i in range(0, n):
        row = []
        for j in range(0, n):
            row.append(random.randint(min, max))
            track_progress(((i * n) + j + 1), n*n)
        matrix.append(row)

    return matrix


@measure_execution_time
def init_vector(n, min, max):
    vector = []

    for i in range(0, n):
        vector.append(random.randint(min, max))
        track_progress(i+1, n)

    return vector


def get_column_i(matrix, i, size):
    column = []

    for iter in range(0, size):
        column.append(matrix[iter][i])

    return column


def print_matrix(n, matrix):
    for i in range(0, n):
        for j in range(0, n):
            print(str(matrix[i][j]), end="\t")
        print("\n")


@measure_execution_time
def pearson_cor(matrix, vector, size):
    rho_vector = []

    for i in range(0, size):
        summ_x = summ_y = summ_x_sq = summ_y_sq = summ_xy = 0
        for j in range(0, size):
            summ_x = summ_x + matrix[j][i]
            summ_y = summ_y + vector[j]
            summ_x_sq = summ_x_sq + (matrix[j][i] ** 2)
            summ_y_sq = summ_y_sq + (vector[j] ** 2)
            summ_xy = summ_xy + (matrix[j][i] * vector[j])
        numerator = (size * summ_xy) - (summ_y * summ_x)
        denominator = ((size * summ_x_sq) - (summ_x ** 2)) * \
            ((size * summ_y_sq) - (summ_y ** 2)) ** 0.5
        rho_vector.append(numerator/denominator)
        track_progress(i+1, size)

    return rho_vector


@measure_execution_time
def pearson_cor_o(matrix, vector, size):
    rho_vector = []

    for i in range(0, size):
        summ_x = summ_y = summ_x_sq = summ_y_sq = summ_xy = 0
        x_col_i = get_column_i(matrix, i, size)
        for j in range(0, size):
            summ_x = summ_x + x_col_i[j]
            summ_y = summ_y + vector[j]
            summ_x_sq = summ_x_sq + (x_col_i[j] ** 2)
            summ_y_sq = summ_y_sq + (vector[j] ** 2)
            summ_xy = summ_xy + (x_col_i[j] * vector[j])
        numerator = (size * summ_xy) - (summ_y * summ_x)
        denominator = ((size * summ_x_sq) - (summ_x ** 2)) * \
            ((size * summ_y_sq) - (summ_y ** 2)) ** 0.5
        rho_vector.append(numerator/denominator)
        track_progress(i+1, size)

    return rho_vector


def main():
    n = 0

    while (n < 1):
        n = int(input("Please enter n: "))

    matrix = init_matrix(n, 1, 10)
    vector = init_vector(n, 1, 10)

    pearson_cor_o(matrix, vector, n)

    matrix = None
    vector = None


if __name__ == "__main__":
    main()
