#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define RANDOM_DEC(num) ((double) (num) * rand() / (RAND_MAX + 1.0))

#define CARD_TYPE_HEART 0
#define CARD_TYPE_PLUM 1
#define CARD_TYPE_SPADES 2
#define CARD_TYPE_SQUARE 3

#define CARD_HEART_A 1
#define CARD_HEART_(num) (num)
#define CARD_HEART_J 11
#define CARD_HEART_Q 12
#define CARD_HEART_K 13
#define CARD_IS_HEART(card) (card >= 1 && card <= 13)

#define CARD_PLUM_A 14
#define CARD_PLUM_(num) (13 + num)
#define CARD_PLUM_J 24
#define CARD_PLUM_Q 25
#define CARD_PLUM_K 26
#define CARD_IS_PLUM(card) (card >= 14 && card <= 26)

#define CARD_SPADES_A 27
#define CARD_SPADES_(num) (26 + num)
#define CARD_SPADES_J 37
#define CARD_SPADES_Q 38
#define CARD_SPADES_K 39
#define CARD_IS_SPADES(card) (card >= 27 && card <= 39)

#define CARD_SQUARE_A 40
#define CARD_SQUARE_(num) (39 + num)
#define CARD_SQUARE_J 50
#define CARD_SQUARE_Q 51
#define CARD_SQUARE_K 52
#define CARD_IS_SQUARE(card) (card >= 40 && card <= 52)


#define CARD_COUNT 52

static int CardType(int card)
{
    if (card >= 1 && card <= 13) {
        return CARD_TYPE_HEART;
    }
    else if (card >= 14 && card <= 26) {
        return CARD_TYPE_PLUM;
    }
    else if (card >= 27 && card <= 39) {
        return CARD_TYPE_SPADES;
    }
    else if (card >= 40 && card <= 52) {
        return CARD_TYPE_SQUARE;
    }
    return -1;
}

static int g_planePro = 0;
int TestPlane(int result[3])
{
    if ( abs(result[0] - result[1]) % 13 == 0 
            && abs(result[0] - result[2]) % 13 == 0) {
        g_planePro++;
    }
    return 0;
}

static int g_categoryPro = 0;
int TestCategory(int result[3])
{
    if (   CardType(result[0]) == CardType(result[1])
            && CardType(result[0]) == CardType(result[2])) {
        g_categoryPro++;
    }
    return 0;
}

static int g_tractorPro = 0;

int TestTractor(int result[3])
{
    int number[3] = {0};
    for (int i = 0; i < 3; i++) {
        number[i] = result[i] % 13;
        if (number[i] == 0) {
            number[i] = 13;
        }
    }

    if (    (number[0] + 1 == number[1] && number[1] + 1 == number[2])
        ||  (number[0] + 1 == number[2] && number[2] + 1 == number[1])
        ||  (number[1] + 1 == number[0] && number[0] + 1 == number[2])
        ||  (number[1] + 1 == number[2] && number[2] + 1 == number[0])
        ||  (number[2] + 1 == number[1] && number[1] + 1 == number[0])
        ||  (number[2] + 1 == number[0] && number[0] + 1 == number[1])) {
        printf("get %d, %d, %d\n", number[0], number[1], number[2]);
        g_tractorPro++;
        return 0;
    }
    for (int i = 0; i < 3; i++) {
        if (number[i] != 12 && number[i] != 13 && number[i] != 1) {
            return -1;
        }
    }
    if (number[0] != number[1] && number[0] != number[2] && number[1] != number[2]) {
        printf("get QKA %d, %d, %d\n", number[0], number[1], number[2]);
        g_tractorPro++;
    }
    return 0;
}

static int g_results[CARD_COUNT + 1] = {0};
int TestProbability()
{
    int result[3] = {0};
    for (int i = 0; i < 3; i++) {
        result[i] = (int)RANDOM_DEC(CARD_COUNT) + 1;
        g_results[result[i]]++;
    }

    TestPlane(result);
    TestCategory(result);
    TestTractor(result);
    return 0;
}

int main()
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < 1000000; i++) {
        TestProbability();
    }

    for (int j = 1; j < CARD_COUNT + 1; j++) {
        //printf("---[%d]: %d, %lf, %lf\n", j, g_results[j], (double)g_results[j] / 100000.0, 1.0 / CARD_COUNT);
    }
    printf("----plane pro %d = %lf, %lf\n", g_planePro, (double) g_planePro / 1000000.0, 1.0 / 13.0 / 13.0);
    printf("----category pro %d = %lf, %lf\n", g_categoryPro, (double) g_categoryPro / 1000000.0, 1.0 / 16.0);
    printf("----tractor pro %d = %lf, %lf\n", g_tractorPro, (double) g_tractorPro / 1000000.0, 1.44 / 51.0);
    return 0;
}

