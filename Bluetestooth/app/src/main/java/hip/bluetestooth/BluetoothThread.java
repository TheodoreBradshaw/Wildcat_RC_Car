package hip.bluetestooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Parcel;
import android.os.ParcelUuid;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

/**
 * Created by User on 11/14/2017.
 */

public class BluetoothThread implements Runnable {

    private Context appctx;
    private BluetoothAdapter bluetoothAdapter;


    static final String ARDUINO_MAC = "84:68:3E:00:01:B7";

    BluetoothThread(Context appctx, BluetoothAdapter adapter) {
        this.appctx = appctx;
        this.bluetoothAdapter = adapter;
    }

    @Override
    public void run() {
        connectToPairedDevice();
    }

    private void connectToPairedDevice() {
        Utils.doLog( "Now begins bluetooth");

        Set<BluetoothDevice> pairedDevices = bluetoothAdapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
            for (BluetoothDevice device : pairedDevices) {
                String deviceName = device.getName();
                String deviceHardwareAddress = device.getAddress();

                if (deviceHardwareAddress.equals(ARDUINO_MAC)) {
                    Utils.doLog("Found the ARDUINO");

                    ParcelUuid[] puuid = device.getUuids();

                    final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");    // FROM THE INTERNET
                    boolean anySucceeded = false;

                    BluetoothSocket sock = null;
                    for (ParcelUuid id : puuid) {
                        // Create rfcomm socket
                        try {
                            //sock = device.createRfcommSocketToServiceRecord(myUUID);
                            sock = device.createInsecureRfcommSocketToServiceRecord(id.getUuid());

                            Utils.doLog( "Trying: " + id.getUuid().toString());

                            try {
                                sock.connect();
                                Utils.doLog( "THIS ONE WORKD: " + id.getUuid().toString());
                                anySucceeded = true;
                                break;
                            } catch (IOException e) {

                                Utils.doLog("Failed to connect");
                                Utils.doLog( e.getMessage());
                            }

                            try {
                                if (sock != null) {
                                    sock.close();
                                }
                            } catch (IOException e1) {}
                        } catch (IOException e) {
                            Utils.doLog( "Failed to create rfcomm socket");
                            Utils.doLog( e.getMessage());
                        }
                   }

                    // Don't believe this lie!
                    if (anySucceeded) {
                        Utils.doLog("Connection successful");
                    } else {
                        Utils.doLog("Nothing but failure");
                    }
                    return;
                }
            }

            Utils.doLog("Failed to find the arduino");
        } else {
            Utils.doLog( "No paired devices");
        }
    }
}
