
## 背景

menbou0202 さんが作成した [Nape Trackball](https://men-bou.net/nape/) というトラックボールと3つのキーが付いたキーボードがあります。

裏面ボタンを使ったレイヤー移動により、8方向に向きを変更することが可能な素晴らしいキーボードなのですが、
私の使用用途では、結構な頻度で向きを変更するのと、どのボタンにどの向きを割り当てたか忘れることが多いので、
裏ボタンを使用せずに、トラックボールの操作で向き変更できるようにしました。

他のトラックボールでも流用できると思いますので、参考になれば幸いです。

## Napeでの使用方法

トラックボールでの向き切り替え方法は、2つの方法があります。

- 任意方向への切り替え
  - 向き切り替えレイヤー(レイヤー 11) にして、使用したい向きにNapeを置いて、トラックボールを下から上に勢いよく回してください。
- configの設定角度分、時計方向 or 半時計方向に回転
  - 向き切り替えレイヤー(レイヤー 11) にして、Napeを時計方向に回転したい場合はトラックボールを左から右に、半時計方向に回転したい場合は右から左に、軽く回してください。

それぞれの閾値に関しては、後述の設定方法を参照してください。

尚、検出のため向き切り替えレイヤーでは、トラックボールが移動しないようにしています。

デフォルトでは、オリジナルのNape同様に8方向をサポートしています。config設定により、より細かい向きの設定が可能です。後述の設定方法を参照してください。

デフォルトでは、向き切り替えレイヤーは裏ボタン3に割り振られていて、そのままでは使用しにくいと思うので、
表のキーに割り振るのをお勧めします。

私はスクロールレイヤー(レイヤー 10)で各キーの長押しに設定しています。([参考](https://github.com/karbou12/zmk-config-nape/commit/c9a35251dd426845dceecc40133c91e8d53b4f88))

## Napeでの設定方法

### driverの変更

fork した zmk-config-nape 上の config/west.yml で membou0202さんの zmk-pmw3610-driver が指定されている箇所を
本driverを指定するように修正してください。([参考](https://github.com/karbou12/zmk-config-nape/commit/4b1e7d576113282eefadaf7c6e0dc235af51ce8e))

### config設定 (Option)

以下のconfig設定が可能です。変更したい場合は、config/boards/shields/nape/nape.conf に追加して適宜値を変更してください。

```
// 向き検出の時間。デフォルトでは、100msで 閾値以上移動したらその向きに設定します。
CONFIG_PMW3610_DIRECTION_DETECTION_SAMPLE_TIME_MS=100

// 任意方向切り替えの距離の閾値。デフォルトでは、100msで 1000pixel(多分)以上移動したらその向きに設定します。
CONFIG_PMW3610_DIRECTION_DETECTION_DISTANCE_THRESHOLD=1000

// 時計方向 or 半時計方向回転の距離の閾値。デフォルトでは、100msで 300pixel以上、1000pixel以下移動したら、その向きに設定します。
// この機能を無効にしたい場合は、0を設定してください。
CONFIG_PMW3610_DIRECTION_SHIFT_THRESHOLD=300

// 向き検出するレイヤー
CONFIG_PMW3610_DIRECTION_DETECTION_LAYER=11

// 1方向あたりの角度。デフォルトでは45度 (360/45 = 8方向) になっています。3度から45度まで設定可能です。
// 360度で割り切れる数にしてください。割り切れない数が設定された場合、内部で割り切れるまで角度を増加します。
CONFIG_PMW3610_DIRECTION_ANGLE=45
```

## アルゴリズム (興味があるかた向け)

向き検出レイヤー上で、一定時間内に閾値距離以上トラックボールが回ったら、移動座標から方向を検出しているだけです。

方向の検出は、[こちらの記事](https://qiita.com/arthur87/items/23d3c896dafbc8223fd5) を参考にしました。

時間と閾値は適当な値ですので、適宜調整してください。

---

以下、オリジナルのREADME。

PMW3610 driver implementation for ZMK with at least Zephyr 3.5

This work is based on [ufan's implementation](https://github.com/ufan/zmk/tree/support-trackpad) of the driver.

## Installation

Only GitHub actions builds are covered here. Local builds are different for each user, therefore it's not possible to cover all cases.

Include this project on your ZMK's west manifest in `config/west.yml`:

```yml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/petejohanson
    - name: inorichi
      url-base: https://github.com/inorichi
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: feat/pointers-move-scroll
      import: app/west.yml
    - name: zmk-pmw3610-driver
      remote: inorichi
      revision: main
  self:
    path: config
```

Then, edit your `build.yml` to look like this, 3.5 is now on main:

```yml
on: [workflow_dispatch]

jobs:
  build:
    uses: zmkfirmware/zmk/.github/workflows/build-user-config.yml@main
```

Now, update your `board.overlay` adding the necessary bits (update the pins for your board accordingly):

```dts
&pinctrl {
    spi0_default: spi0_default {
        group1 {
            psels = <NRF_PSEL(SPIM_SCK, 0, 8)>,
                <NRF_PSEL(SPIM_MOSI, 0, 17)>,
                <NRF_PSEL(SPIM_MISO, 0, 17)>;
        };
    };

    spi0_sleep: spi0_sleep {
        group1 {
            psels = <NRF_PSEL(SPIM_SCK, 0, 8)>,
                <NRF_PSEL(SPIM_MOSI, 0, 17)>,
                <NRF_PSEL(SPIM_MISO, 0, 17)>;
            low-power-enable;
        };
    };
};

&spi0 {
    status = "okay";
    compatible = "nordic,nrf-spim";
    pinctrl-0 = <&spi0_default>;
    pinctrl-1 = <&spi0_sleep>;
    pinctrl-names = "default", "sleep";
    cs-gpios = <&gpio0 20 GPIO_ACTIVE_LOW>;

    trackball: trackball@0 {
        status = "okay";
        compatible = "pixart,pmw3610";
        reg = <0>;
        spi-max-frequency = <2000000>;
        irq-gpios = <&gpio0 6 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;

        /*   optional features   */
        // snipe-layers = <1>;
        // scroll-layers = <2 3>;
        // automouse-layer = <4>;
    };
};

/ {
  trackball_listener {
    compatible = "zmk,input-listener";
    device = <&trackball>;

  };
};
```

Now enable the driver config in your `board.config` file (read the Kconfig file to find out all possible options):

```conf
CONFIG_SPI=y
CONFIG_INPUT=y
CONFIG_ZMK_MOUSE=y
CONFIG_PMW3610=y
```
