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


def init_empty_vector(n):
    vector = []

    for i in range(0, n):
        vector.append(0)

    return vector


def print_matrix(n, matrix):
    for i in range(0, n):
        for j in range(0, n):
            print(str(matrix[i][j]), end="\t")
        print("\n")

#
# TODO: Complete the following function.
# Use lab01.c as your guide.


def pearson_cor(matrix, vector, size):
    rho_vector = init_empty_vector(size)

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
