package org.cocos2dx.cpp;

import java.io.File;

import jp.co.septeni.original.leadblow.BuildConfig;
import jp.co.septeni.original.leadblow.FacebookActivity;
import jp.co.septeni.original.leadblow.R;
import jp.co.septeni.original.leadblow.billing.IabHelper;
import jp.co.septeni.original.leadblow.billing.IabHelper.OnIabPurchaseFinishedListener;
import jp.co.septeni.original.leadblow.billing.Purchase;
import jp.co.septeni.original.leadblow.billing.IabHelper.OnIabSetupFinishedListener;
import jp.co.septeni.original.leadblow.billing.IabResult;
import android.app.NativeActivity;
import android.content.ComponentName;
import android.content.ContentValues;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;
import android.util.Log;

import com.facebook.LoggingBehavior;
import com.facebook.Session;
import com.facebook.Session.NewPermissionsRequest;
import com.facebook.Session.StatusCallback;
import com.facebook.SessionState;
import com.facebook.Settings;

public class Cocos2dxActivity extends NativeActivity{

	private static Cocos2dxActivity that = null;
	private static final String PERMISSION = "publish_actions";
	private IabHelper iabHelper;
	public static int IAB_REQUEST_CODE = 10001;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		that = this;

		Settings.addLoggingBehavior(LoggingBehavior.INCLUDE_ACCESS_TOKENS);
        Session session = Session.getActiveSession();
        if (session == null) {
            if (savedInstanceState != null) {
                session = Session.restoreSession(this, null, null, savedInstanceState);
            }
            if (session == null) {
                session = new Session(this);
            }
            Session.setActiveSession(session);
        }

        String base64PublicKey = getString(R.string.iab_key);
        iabHelper = new IabHelper(this, base64PublicKey);
		if (BuildConfig.DEBUG) {
			iabHelper.enableDebugLogging(true);
		}
        iabHelper.startSetup(new OnIabSetupFinishedListener() {
			@Override
			public void onIabSetupFinished(IabResult result) {
				if (result.isFailure()) {
					Log.d("", result.getMessage());
				}
			}
		});

		//For supports translucency
		
		//1.change "attribs" in cocos\2d\platform\android\nativeactivity.cpp
		/*const EGLint attribs[] = {
	            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,  
	            //EGL_BLUE_SIZE, 5,   -->delete 
	            //EGL_GREEN_SIZE, 6,  -->delete 
	            //EGL_RED_SIZE, 5,    -->delete 
	            EGL_BUFFER_SIZE, 32,  //-->new field
	            EGL_DEPTH_SIZE, 16,
	            EGL_STENCIL_SIZE, 8,
	            EGL_NONE
	    };*/
		
		//2.Set the format of window
		// getWindow().setFormat(PixelFormat.TRANSLUCENT);
		
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (iabHelper.handleActivityResult(requestCode, resultCode, data)) {
			return;
		}
		super.onActivityResult(requestCode, resultCode, data);
		Session.getActiveSession().onActivityResult(this, requestCode,
				resultCode, data);
	}

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        Session session = Session.getActiveSession();
        Session.saveSession(session, outState);
    }

	public static void tweet(String message, String filePath) {
		File file = new File(filePath);
		file.setReadable(true, false);
		// if needed
		// that.registerMedia(file);
		Intent intent = new Intent(Intent.ACTION_SEND);
		intent.setType("image/jpeg");
		intent.putExtra(Intent.EXTRA_TEXT, message);
		intent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file));
		String packageName = "com.twitter.android";
		PackageManager pm = that.getPackageManager();
		for (ResolveInfo info :pm.queryIntentActivities(intent, 0)) {
			if (info.activityInfo.applicationInfo.packageName.equals(packageName)) {
				intent.setComponent(new ComponentName(packageName, info.activityInfo.name));
				that.startActivity(intent);
				return;
			}
		}
		that.startActivity(Intent.createChooser(intent, "共有"));
	}
	
	private void registerMedia(File file) {
		ContentValues values = new ContentValues();
		values.put(Images.Media.MIME_TYPE, "image/jpeg");
		values.put("_data", file.getAbsolutePath());
		getContentResolver().insert(
				MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
	}
	
	public static void facebook(String message, String filePath) {
		that.tryPost(filePath, message);
	}

	private void tryPost(String filePath, String message) {
		Session session = Session.getActiveSession();
		if (session.isOpened()) {
			if (session.getPermissions().contains(PERMISSION)) {
				showPostActivity(message, filePath);
			} else {
				requestPermission(session, filePath, message);
			}
		} else {
			login(session, filePath, message);
		}
	}

	private void showPostActivity(String message, String filePath) {
		Intent intent = new Intent(that, FacebookActivity.class);
		intent.putExtra("message", message);
		intent.putExtra("filePath", filePath);
		that.startActivity(intent);
	}

	private void requestPermission(Session session, final String filePath,
			final String message) {
		session.requestNewPublishPermissions(new NewPermissionsRequest(this,
				PERMISSION).setCallback(new StatusCallback() {
			@Override
			public void call(Session session, SessionState state,
					Exception exception) {
				if (state == SessionState.OPENED_TOKEN_UPDATED) {
					tryPost(filePath, message);
				}
			}
		}));
	}

	private void login(Session session, final String filePath, final String message) {
		StatusCallback callback = new StatusCallback() {
			@Override
			public void call(Session session, SessionState state, Exception exception) {
				if (state == SessionState.OPENED) {
					tryPost(filePath, message);
				}
			}
		};
        if (!session.isClosed()) {
            session.openForRead(new Session.OpenRequest(this).setCallback(callback));
        } else {
            Session.openActiveSession(this, true, callback);
        }
    }

	public static String getCachePath() {
		return that.getCacheDir().getAbsolutePath();
	}

	public static void openReviewPage() {
		Intent intent = new Intent(Intent.ACTION_VIEW,
				Uri.parse("https://play.google.com/store/apps/details?id="
						+ that.getPackageName() + "&hl=ja"));
		that.startActivity(intent);
	}

	public static void purchase(String productID) {
		that.launchPurchaseFlow(productID);
	}

	public static native void onPurchase(String result);

	private void launchPurchaseFlow(String sku) {
		iabHelper.launchPurchaseFlow(this, sku, IAB_REQUEST_CODE, new OnIabPurchaseFinishedListener() {
			@Override
			public void onIabPurchaseFinished(IabResult result, Purchase info) {
				if (result.isSuccess()) {
					iabHelper.consumeAsync(info, null);
				}
				onPurchase(result.getMessage());
			}
		});
	}

}
