package hip.bluetestooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    BluetoothAdapter bluetoothAdapter;

    static final int BLUETOOTH_ACTIVITY_ID = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView text = findViewById(R.id.mytext);

        Button myButton = findViewById(R.id.button);
        myButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                text.setText("YOU PRESSED THE BUTTON");
            }
        });

        // If this phone, doesn't have bluetooth, abort
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Utils.doLog( "Bluetooth is not supported, goodbye");
            return;
        }

        // If ths phone doesn't have bluetooth enabled, try to enable it
        if (!bluetoothAdapter.isEnabled()) {
            Utils.doLog( "Bluetooth is not enabled");
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, BLUETOOTH_ACTIVITY_ID);
            return;
        }

        Thread thread = new Thread(new BluetoothThread(this, bluetoothAdapter));
        thread.start();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == BLUETOOTH_ACTIVITY_ID) {
            if (resultCode == RESULT_OK) {
                Thread thread = new Thread(new BluetoothThread(this, bluetoothAdapter));
                thread.run();
            } else {
                Utils.doLog( "User still refused to turn on bluetooth");
            }
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
    }
}
