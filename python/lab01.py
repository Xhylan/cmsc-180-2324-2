import random


def init_matrix(n, min, max):
    matrix = []

    for i in range(0, n):
        row = []
        for j in range(0, n):
            row.append(random.randint(min, max))
        matrix.append(row)

    return matrix


def init_vector(n, min, max):
    vector = []

    for i in range(0, n):
        vector.append(random.randint(min, max))

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
#
# TODO: Complete the following function.
# Use lab01.c as your guide.


def pearson_cor(matrix, vector, size):
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

    return rho_vector


def main():
    n = 0

    while (n < 1):
        n = int(input("Please enter n: "))

    matrix = init_matrix(n, 1, 10)
    print_matrix(n, matrix)
    vector = init_vector(n, 1, 10)

    pearson_cor(matrix, vector, n)

    matrix = None
    vector = None


if __name__ == "__main__":
    main()
