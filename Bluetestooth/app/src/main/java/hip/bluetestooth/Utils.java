package hip.bluetestooth;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by User on 11/14/2017.
 */

public class Utils {

    private static final boolean DO_DEBUG_PRINT = true;
    private static  boolean DO_LONG_TOAST = false;
    public static void doLog(String text) {
        if (DO_DEBUG_PRINT) {
            //Toast.makeText(ctx, text,
            //        DO_LONG_TOAST ? Toast.LENGTH_LONG : Toast.LENGTH_SHORT).show();
            Log.d("BLU", text);
        }


    }
}
