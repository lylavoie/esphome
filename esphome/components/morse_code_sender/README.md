# Morse Code Sender

The `morse_code_sender` component keys a [switch](https://esphome.io/components/switch/) on and off to send
Morse code. Any switch works as the output: a GPIO switch driving an LED, buzzer, relay or transmitter key
line, a template switch, and so on.

The component does not translate text to Morse. You send it a pattern made of three characters:

| Character | Meaning |
|-----------|---------|
| `.` | Dot: switch on for 1 unit |
| `-` | Dash: switch on for 3 units |
| ` ` (space) | Extra silence of 2 units (letter or word gap) |

## Configuration

```yaml
switch:
  - platform: gpio
    pin: GPIO2
    id: key_out

morse_code_sender:
  id: morse
  switch_id: key_out
  wpm: 20
```

### Configuration variables

- **id** (*Optional*, [ID](https://esphome.io/guides/configuration-types#config-id)): ID for the component,
  needed to target it with actions.
- **switch_id** (**Required**, [ID](https://esphome.io/guides/configuration-types#config-id)): The switch to use
  as the keying output.
- **wpm** (*Optional*, int): Keying rate in words per minute, 1 to 100. Defaults to `20`.

## Timing

Timing follows the standard Morse ratios, with the unit length taken from the PARIS standard word:

```text
unit (ms) = 1200 / wpm
```

At 20 WPM one unit is 60 ms.

| Element | Duration |
|---------|----------|
| Dot | 1 unit on |
| Dash | 3 units on |
| Gap after every dot or dash (automatic) | 1 unit off |
| Each space | 2 more units off |

So the spacing comes out as follows:

- **Between the dots and dashes of one letter:** nothing to add, the 1-unit gap is automatic.
- **Between letters:** one space, for a 3-unit gap.
- **Between words:** three spaces, for a 7-unit gap.

The switch is turned off at boot and after each element.

> **Note:** the component runs in ESPHome's main loop, which ticks about every 16 ms, so each element can be
> up to ~16 ms late. That is fine up to about 30 WPM; faster rates will sound uneven.

## Actions

### `morse_code_sender.send_keys`

Adds a pattern to the send queue. If the component is already sending, the new keys go after the ones already
queued, so several calls play back to back.

```yaml
on_press:
  - morse_code_sender.send_keys:
      id: morse
      keys: "... --- ..."   # SOS
```

- **id** (**Required**, [ID](https://esphome.io/guides/configuration-types#config-id)): The
  `morse_code_sender` to use.
- **keys** (**Required**, [templatable](https://esphome.io/automations/templates), string): The pattern to
  send, made only of `.`, `-` and spaces.

A fixed `keys` string with any other character fails config validation. A lambda's value can't be checked at
build time, so any other characters in it are skipped and a warning is logged:

```yaml
- morse_code_sender.send_keys:
    id: morse
    keys: !lambda 'return id(my_text).state;'
```

## Full example

Send "SOS SOS" on an LED from a Home Assistant button:

```yaml
switch:
  - platform: gpio
    pin: GPIO2
    id: led
    internal: true

morse_code_sender:
  id: morse
  switch_id: led
  wpm: 15

button:
  - platform: template
    name: Send SOS
    on_press:
      - morse_code_sender.send_keys:
          id: morse
          keys: "... --- ...   ... --- ..."
```

## C++ API

From a lambda:

- `id(morse).send_keys("-.-.");` queues a pattern.
- `id(morse).is_sending()` returns `true` while keys are still being sent.
