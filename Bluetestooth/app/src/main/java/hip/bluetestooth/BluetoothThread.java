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
                    for (ParcelUuid id : puuid) {
                        Utils.doLog( id.getUuid().toString());
                    }


                    // Create rfcomm socket
                    BluetoothSocket sock;
                    //BluetoothSocket sock = device.createRfcommSocketToServiceRecord("2a37");
                    try {
                        //sock = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("0000-00-00-00-00180d"));
                        sock = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("0000180d-0000-1000-8000-00805f9b34fb"));
                    } catch (IOException e) {
                        Utils.doLog( "Failed to create rfcomm socket");
                        Utils.doLog( e.getMessage());
                        return;
                    }

                    // Connect to socket
                    for (ParcelUuid id : puuid) {
                        Utils.doLog( "Trying: " + id.getUuid().toString());

                        try {
                            sock.connect();
                            Utils.doLog( "THIS ONE WORKD: " + id.getUuid().toString());
                        } catch (IOException e) {

                            Utils.doLog("Failed to connect");
                            Utils.doLog( e.getMessage());
                        }
                    }


                    try {
                        sock.close();
                    } catch (IOException e1) {}

                    //Utils.doLog( "Connection successful");
                    return;
                }
            }

            Utils.doLog("Failed to find the arduino");
        } else {
            Utils.doLog( "No paired devices");
        }
    }
}
