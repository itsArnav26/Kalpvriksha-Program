#include <stdio.h>
#include <stdlib.h>

int **generateMatrix(int sizeOfMatrix);
void displayMatrix(int sizeOfMatrix, int **matrix);
void rotateMatrix(int sizeOfMatrix, int **matrix);
void swapPointers(int **first, int **second);
void freeMatrix(int **matrix, int count);
int smoothMatrix(int sizeOfMatrix, int **matrix);

int main(void)
{
    int sizeOfMatrix;

    printf("Enter the size of matrix (2 to 10): ");
    scanf("%d", &sizeOfMatrix);

    if (sizeOfMatrix < 2 || sizeOfMatrix > 10)
    {
        printf("Invalid size\n");
        return 0;
    }

    int **matrix = generateMatrix(sizeOfMatrix);

    if (matrix == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    printf("\nGenerated Matrix:\n");
    displayMatrix(sizeOfMatrix, matrix);

    rotateMatrix(sizeOfMatrix, matrix);

    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(sizeOfMatrix, matrix);

    if (smoothMatrix(sizeOfMatrix, matrix) == 0)
    {
      printf("Memory Allocation Failed.");
      return 0;
    }

    printf("Matrix after Applying Smoothing Filter: \n");
    displayMatrix(sizeOfMatrix, matrix);

    // free all the memory
    freeMatrix(matrix, sizeOfMatrix * sizeOfMatrix);

    return 0;
}

int **generateMatrix(int sizeOfMatrix)
{
    int totalCells = sizeOfMatrix * sizeOfMatrix;

    int **matrix = malloc(totalCells * sizeof *matrix);

    if (matrix == NULL)
    {
        return NULL;
    }

    for (int row = 0; row < sizeOfMatrix; row++)
    {
        for (int column = 0; column < sizeOfMatrix; column++)
        {
            int position = row * sizeOfMatrix + column;

            *(matrix + position) = malloc(sizeof **matrix);

            if (*(matrix + position) == NULL)
            {   // free all the memory
                freeMatrix(matrix, position);
                return NULL;
            }

            **(matrix + position) = (row + column) * 10;
        }
    }

    return matrix;
}

void displayMatrix(int sizeOfMatrix, int **matrix)
{
    for (int row = 0; row < sizeOfMatrix; row++)
    {
        for (int column = 0; column < sizeOfMatrix; column++)
        {
            int position = row * sizeOfMatrix + column;

            printf("%d  ", **(matrix + position));
        }

        printf("\n");
    }
}

void swapPointers(int **first, int **second)
{
    int *temporary = *first;
    *first = *second;
    *second = temporary;
}

void rotateMatrix(int sizeOfMatrix, int **matrix)
{
    // Transpose by swapping pointers
    for (int row = 0; row < sizeOfMatrix; row++)
    {
        for (int column = row + 1; column < sizeOfMatrix; column++)
        {
            int firstPosition = row * sizeOfMatrix + column;
            int secondPosition = column * sizeOfMatrix + row;

            swapPointers(matrix + firstPosition,
                         matrix + secondPosition);
        }
    }

    // Reverse each row by swapping pointers
    for (int row = 0; row < sizeOfMatrix; row++)
    {
        int left = 0;
        int right = sizeOfMatrix - 1;

        while (left < right)
        {
            int firstPosition = row * sizeOfMatrix + left;
            int secondPosition = row * sizeOfMatrix + right;

            swapPointers(matrix + firstPosition,
                         matrix + secondPosition);

            left++;
            right--;
        }
    }
}

void freeMatrix(int **matrix, int count)
{
    for (int position = 0; position < count; position++)
    {
        free(*(matrix + position));
    }

    free(matrix);
}

int smoothMatrix(int sizeOfMatrix, int **matrix)
{
    int *previousRow = malloc(sizeOfMatrix * sizeof *previousRow);
    int *currentRow = malloc(sizeOfMatrix * sizeof *currentRow);
    int *nextRow = malloc(sizeOfMatrix * sizeof *nextRow);

    if (previousRow == NULL || currentRow == NULL || nextRow == NULL)
    {
        free(previousRow);
        free(currentRow);
        free(nextRow);
        return 0;
    }

    //  Load the first two rows
    for (int column = 0; column < sizeOfMatrix; column++)
    {
        *(currentRow + column) = **(matrix + column);

        *(nextRow + column) =
            **(matrix + sizeOfMatrix + column);
    }

    for (int row = 0; row < sizeOfMatrix; row++)
    {
        for (int column = 0; column < sizeOfMatrix; column++)
        {
            int sum = 0;
            int count = 0;

            for (int neighborColumn = column - 1;
                 neighborColumn <= column + 1;
                 neighborColumn++)
            {
                // Skip columns outside the matrix
                if (neighborColumn < 0 ||
                    neighborColumn >= sizeOfMatrix)
                {
                    continue;
                }

                // values from the previous row
                if (row > 0)
                {
                    sum += *(previousRow + neighborColumn);
                    count++;
                }

                // Includes the current cell and horizontal neighbors of currentRow
                sum += *(currentRow + neighborColumn);
                count++;

                // values from the next row
                if (row + 1 < sizeOfMatrix)
                {
                    sum += *(nextRow + neighborColumn);
                    count++;
                }
            }

            int position = row * sizeOfMatrix + column;
            // Calculating Average
            **(matrix + position) = sum / count;
        }

        // laod currentRow and previousRow for next Iteration
        if (row + 1 < sizeOfMatrix)
        {
            for (int column = 0; column < sizeOfMatrix; column++)
            {
                *(previousRow + column) = *(currentRow + column);
                *(currentRow + column) = *(nextRow + column);
            }

            //Load the nextRow
            if (row + 2 < sizeOfMatrix)
            {
                for (int column = 0; column < sizeOfMatrix; column++)
                {
                    int position =
                        (row + 2) * sizeOfMatrix + column;

                    *(nextRow + column) = **(matrix + position);
                }
            }
        }
    }
    // free all the memory
    free(previousRow);
    free(currentRow);
    free(nextRow);

    return 1;
}