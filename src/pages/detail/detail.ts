import { Component, NgZone } from '@angular/core';
import { NavController, NavParams , AlertController } from 'ionic-angular';
import { BLE } from '@ionic-native/ble';

//const BUTTON_SERVICE = 'a000';
//////const BUTTON_SERVICE = 'BLE UART-MINEW';
//const BUTTON_STATE_CHARACTERISTIC = 'a001';

const LED_SERVICE = 'a000';
const LED_CHARACERISTIC = 'a001';

const UART_SERVICE = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
const UART_TX_CHARACTERISTIC = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';
const UART_RX_CHARACTERISTIC = '6e400002-b5a3-f393-e0a9-e50e24dcca9e';


const LIGHTBULB_SERVICE = 'ff10';
const SWITCH_CHARACTERISTIC = 'ff11';
const DIMMER_CHARACTERISTIC = 'ff12';

@Component({
  selector: 'page-detail',
  templateUrl: 'detail.html',
})
export class DetailPage {

  peripheral: any = {};
  power: boolean;
  brightness: number;
  statusMessage: string;

  constructor(public navCtrl: NavController,
              public navParams: NavParams,
              private ble: BLE,
              private alertCtrl: AlertController,
              private ngZone: NgZone) {

    let device = navParams.get('device');

    this.setStatus('Connecting to ' + device.name || device.id);

    this.ble.connect(device.id).subscribe(
      peripheral => this.onConnected(peripheral),
      peripheral => this.showAlert('Disconnected', 'The peripheral unexpectedly disconnected')
    );

  }

  onConnected(peripheral) {

    this.peripheral = peripheral;
    this.setStatus('Connected to ' + (peripheral.name || peripheral.id));

    // Update the UI with the current state of the switch characteristic
    this.ble.read(this.peripheral.id, LED_SERVICE, LED_CHARACERISTIC).then(
      buffer => {
        let data = new Uint8Array(buffer);
        console.log('switch characteristic ' + data[0]);
        this.ngZone.run(() => {
            this.power = data[0] !== 0;
        });
      }
    )

    // Update the UI with the current state of the dimmer characteristic
     /* this.ble.read(this.peripheral.id, LIGHTBULB_SERVICE, DIMMER_CHARACTERISTIC).then(
     buffer => {
        let data = new Uint8Array(buffer);
        console.log('dimmer characteristic ' + data[0]);
        this.ngZone.run(() => {
          this.brightness = data[0];
        });
      }
    )*/
  }

  onPowerSwitchChange(event) {
    // console.log('onPowerSwitchChange');
//*********************************** */
   // var data = new Uint8Array(1);

//*********************************** */
   /* data[0] = this.power ? 1 : 0;

     this.ble.write(this.peripheral.id, LED_SERVICE , LED_CHARACERISTIC , data.buffer);
      console.log("Sending: " + data[0]); */

  }

  setBrightness(event) {
    console.log("this.brightness: " + this.brightness);
       var data = new Uint8Array([this.brightness]);
       console.log("Sending: " + data[0]);
    this.ble.write(this.peripheral.id, LED_SERVICE, LED_CHARACERISTIC, data.buffer).then(
      () => this.setStatus('Set brightness to ' + this.brightness),
      e => this.showAlert('Unexpected Error', 'Error updating dimmer characteristic ' + e)
    );
  }

  // Disconnect peripheral when leaving the page
  ionViewWillLeave() {
    console.log('ionViewWillLeave disconnecting Bluetooth');
    this.ble.disconnect(this.peripheral.id).then(
      () => console.log('Disconnected ' + JSON.stringify(this.peripheral)),
      () => console.log('ERROR disconnecting ' + JSON.stringify(this.peripheral))
    )
  }

  showAlert(title, message) {
    let alert = this.alertCtrl.create({
      title: title,
      subTitle: message,
      buttons: ['OK']
    });
    alert.present();
  }

  setStatus(message) {
    console.log(message);
    this.ngZone.run(() => {
      this.statusMessage = message;
    });
  }

}
