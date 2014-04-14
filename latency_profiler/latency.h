#ifndef LATENCY_H
#define LATENCY_H

/* Using P4.3 as second debug PIN */
#define TRIGGER_PIN          (BIT3)
#define TRIGGER_PIN_INIT      (P4DIR = 0xFF)

#define TRIGGER_PIN_ON       (P4OUT |= TRIGGER_PIN)
#define TRIGGER_PIN_OFF       (P4OUT &= ~TRIGGER_PIN)
#define TRIGGER_PIN_TOGGLE    (P4OUT ^= TRIGGER_PIN)

#endif /* LATENCY_H */
