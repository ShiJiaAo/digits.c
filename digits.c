/**
 * CS1010 Semester 1 AY21/22
 * Assignment 9: Digits
 *
 * @file: digits.c
 * @author: Shi Jiaao (Group D09)
 */

#include "cs1010.h"
#include <stdbool.h>

#define NCOLS 28
#define NROWS 28
#define WHITE '.'
#define BLACK '#'
#define K 5

// defining structs
typedef struct digit
{
  char **sample;
  long label;
} digit_t;

typedef struct neighbor
{
  long neighbor;
  long distance;
} neighbor_t;

/**
 * Reads in a training/testing sample.
 *
 * @param[in] nrows The number of rows in the array representing the sample
 * @return NULL if memory allocation fails, otherwise returns sample, a pointer to the first element
 * of the sample that has been read in
 */
char **read_sample(size_t nrows)
{
  char **sample = calloc(nrows, sizeof(char *));
  if (sample == NULL)
  {
    return NULL;
  }
  for (size_t i = 0; i < nrows; i += 1)
  {
    sample[i] = cs1010_read_word();
    if (sample[i] == NULL)
    {
      for (size_t j = 0; j < i; j += 1)
      {
        free(sample[j]);
      }
      free(sample);
      return NULL;
    }
  }
  return sample;
}

/**
 * Finds the distance between a test sample and a training sample.
 *
 * @param[in] test_sample The test sample to be compared
 * @param[in] training_sample The training sample to be compared
 * @param[in] prevailing_max_dist The current maximum distance in the k-nearest neighbors list
 * @return -1 if distance > prevailing_max_dist, otherwise
 * returns distance The distance (represented as a long) between the 2 samples
 */
long find_distance(char **test_sample, char **training_sample, long prevailing_max_dist)
{
  long distance = 0;
  for (size_t i = 0; i < NROWS; i += 1)
  {
    for (size_t j = 0; j < NCOLS; j += 1)
    {
      if (prevailing_max_dist != -1 && distance > prevailing_max_dist)
      {
        return -1;
      }
      if (test_sample[i][j] != training_sample[i][j])
      {
        distance += 1;
      }
    }
  }
  return distance;
}

/**
 * Inserts a neighboring digit and the distance to it into a struct within the struct array neighbor
 *
 * @param[in] len The number of elements in the struct array 'neighbor'
 * @param[in, out] neighbor The struct array containing the struct that the digit and distance will be
 * inserted into
 * @param[in] digit The digit to be inserted
 * @param[in] distance The distance value to be inserted
 */
void insert(long len, neighbor_t neighbor[len], long digit, long distance)
{
  long i = 0;
  while (i < len && neighbor[i].distance != -1 && distance > neighbor[i].distance)
  {
    i += 1;
  }
  while (i < len && distance == neighbor[i].distance && neighbor[i].neighbor < digit)
  {
    i += 1;
  }
  for (long j = len - 1; j > i; j -= 1)
  {
    neighbor[j].neighbor = neighbor[j - 1].neighbor;
    neighbor[j].distance = neighbor[j - 1].distance;
  }
  if (i < len)
  {
    neighbor[i].neighbor = digit;
    neighbor[i].distance = distance;
  }
}

/**
 * Finds the digit that the program will recognise as the digit in the test sample
 *
 * @param[in] len The number of elements in the struct array 'neighbor'
 * @param[in] neighbor The struct array containing the k-nearest neighbors
 *
 * @return recognised_digit The digit that the program recognises as the digit in the test sample
 */
long find_recognised_digit(long len, neighbor_t neighbor[len])
{
  long recognised_digit = -1;
  long max_occurance = -1;
  // occurances[j][1] will be the number of occurances of the digit j, and
  // occurances[j][2] will be the minimum distance of all occurances of digit j
  long occurances[10][2] = {
    {0}
  };
  for (long i = 0; i < len; i += 1)
  {
    // update count
    occurances[neighbor[i].neighbor][0] += 1;
    //update min. distance
    if (occurances[neighbor[i].neighbor][0] == 1)
    {
      occurances[neighbor[i].neighbor][1] = neighbor[i].distance;
    }
    if (occurances[neighbor[i].neighbor][0] > 1 &&
        neighbor[i].distance < occurances[neighbor[i].neighbor][1])
    {
      occurances[neighbor[i].neighbor][1] = neighbor[i].distance;
    }
  }
  for (long j = 0; j < 10; j += 1)
  {
    if (occurances[j][0] > max_occurance)
    {
      recognised_digit = j;
      max_occurance = occurances[j][0];
    }
    if (occurances[j][0] == max_occurance)
    {
      if (occurances[j][1] < occurances[recognised_digit][1] ||
          (occurances[j][1] == occurances[recognised_digit][1] && j < recognised_digit))
      {
        recognised_digit = j;
      }
    }
  }
  return recognised_digit;
}

/**
 * Prints out the correct digit followed by the digit recognised by the program
 *
 * @param[in] correct_digit The correct digit
 * @param[in] recognised_digit The digit recognised by the program
 */
void print_digits(long correct_digit, long recognised_digit)
{
  cs1010_print_long(correct_digit);
  cs1010_print_string(" ");
  cs1010_println_long(recognised_digit);
}

/**
 * Finds the accuracy of the program
 *
 * @param[in] count The number of times the program correctly recognised a digit
 * @param[in] m The number of test samples
 *
 * @return a double value representing the accuracy of the program
 */
double find_accuracy(long count, size_t m)
{
  return count * 1.0 / (long)m * 100;
}

/**
 * Finds the k-nearest neighbors, finds the recognised digit, and prints out the correct digit and the 
 * recognised digit
 *
 * @param[in] testing A pointer to the struct array containing all test samples
 * @param[in] training A pointer to the struct array containing all training samples
 * @param[in] n The number of training samples
 * @param[in] m The number of test samples
 * @param[in, out] count A pointer to the variable 'count'. 'count' represents the number of times the
 * program correctly recognises the digit in a test case
 */
void get_neighbors_and_print(digit_t *testing, digit_t *training, size_t n, size_t m, long *count)
{
  neighbor_t neighbor[K];
  for (size_t i = 0; i < m; i += 1)
  {
    for (size_t p = 0; p < K; p += 1)
    {
      neighbor[p].neighbor = -1;
      neighbor[p].distance = -1;
    }
    for (size_t j = 0; j < n; j += 1)
    {
      long distance = find_distance(testing[i].sample, training[j].sample, neighbor[4].distance);
      if (distance != -1)
      {
        insert(K, neighbor, training[j].label, distance);
      }
    }
    long correct_digit = testing[i].label;
    long recognised_digit = find_recognised_digit(K, neighbor);
    if (correct_digit == recognised_digit)
    {
      *count += 1;
    }
    print_digits(correct_digit, recognised_digit);
  }
}

/**
 * Frees a struct array from the heap
 * 
 * @param[in, out] structure A pointer to the struct array to be freed
 * @param[in] num_of_structs The number of structs in the struct array
 * @param[in] nrows The number of rows of each 2D array contained within each struct array
 */
void free_structure(digit_t *structure, size_t num_of_structs, size_t nrows)
{
  for (size_t i = 0; i < num_of_structs; i += 1)
  {
    for (size_t j = 0; j < nrows; j += 1)
    {
      free(structure[i].sample[j]);
    }
    free(structure[i].sample);
  }
  free(structure);
}

int main()
{
  // reading in training samples
  size_t n = cs1010_read_size_t(); // n is number of training samples
  digit_t *training = calloc(n, sizeof(digit_t));
  if (training == NULL)
  {
    cs1010_println_string("unable to allocate memory");
    return 1;
  }
  for (size_t i = 0; i < n; i += 1)
  {
    training[i].label = cs1010_read_long();
    training[i].sample = read_sample(NROWS);
    if (training[i].sample == NULL)
    {
      free_structure(training, i, NROWS);
      cs1010_println_string("unable to allocate memory");
      return 1;
    }
  }

  // reading in test samples
  size_t m = cs1010_read_size_t(); // m is number of test samples
  digit_t *testing = calloc(m, sizeof(digit_t));
  if (testing == NULL)
  {
    free_structure(training, n, NROWS);
    cs1010_println_string("unable to allocate memory");
    return 1;
  }
  for (size_t i = 0; i < m; i += 1)
  {
    testing[i].label = cs1010_read_long();
    testing[i].sample = read_sample(NROWS);
    if (testing[i].sample == NULL)
    {
      free_structure(training, n, NROWS);
      free_structure(testing, i, NROWS);
      cs1010_println_string("unable to allocate memory");
      return 1;
    }
  }

  // finding distance and printing
  long count = 0;
  get_neighbors_and_print(testing, training, n, m, &count);

  // finding accuracy
  double accuracy = find_accuracy(count, m);
  cs1010_println_double(accuracy);

  // freeing
  free_structure(training, n, NROWS);
  free_structure(testing, m, NROWS);
}
