#include <assert.h>
#include <math.h>
#include <unistd.h>

#define LOW 0
#define HIGH 1

#define INPUT 0
#define OUTPUT 1

#define NPINS 20
struct pin_t { int mode; int val; };
static struct pin_t pins[NPINS];

void clear() {
    printf("\x1b[2J");
}

void print_pin(struct pin_t *pin) {
    printf(" %s ", pin->val == HIGH ? "o" : "-");
}

void display(int cnt,
             const int *hours, int nhours,
             const int *mins, int nmins,
             const int *secs, int nsecs) {
    int i = 0;
    struct pin_t pin;

    clear();
    printf("Iteration:  %d\nTime (sec): %d\n\n", cnt, (unsigned int) (cnt / 1000));

    printf("Secs:  ");
    for (i = 0; i < nsecs; i++) {
        pin = pins[secs[i]];
        assert(pin.mode == OUTPUT);
        print_pin(&pin);
    }
    printf("\n");

    printf("Mins:  ");
    for (i = 0; i < nmins; i++) {
        pin = pins[mins[i]];
        assert(pin.mode == OUTPUT);
        print_pin(&pin);
    }
    printf("\n");

    printf("Hours: ");
    for (i = 0; i < nhours; i++) {
        pin = pins[hours[i]];
        assert(pin.mode == OUTPUT);
        print_pin(&pin);
    }
    printf("\n");
}

void pinMode(int pin, int mode) {
    pins[pin].mode = mode;
}

unsigned long millis() {
    static unsigned long time = 0;
    time += 1;
    return time;
}

void delay(int ms) {
    sleep((int) ceil(ms / 1000.0));
}

int digitalRead(int pin) {
    assert (pins[pin].mode == INPUT);
    return LOW;
}

void digitalWrite(int pin, int val) {
    assert (pins[pin].mode == OUTPUT);
    pins[pin].val = val;
}

int bitRead(int val, int bit) {
    return (val >> bit) & 1;
}
