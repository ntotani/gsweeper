package org.cocos2dx.cpp;

import java.io.File;
import java.io.FileNotFoundException;

import android.app.NativeActivity;
import android.content.ComponentName;
import android.content.ContentValues;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;

import com.facebook.LoggingBehavior;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.Session.NewPermissionsRequest;
import com.facebook.Session.StatusCallback;
import com.facebook.SessionState;
import com.facebook.Settings;

public class Cocos2dxActivity extends NativeActivity{

	private static Cocos2dxActivity that = null;
	private static final String PERMISSION = "publish_actions";
	private Handler handler;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		that = this;
		handler = new Handler();

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
        super.onActivityResult(requestCode, resultCode, data);
        Session.getActiveSession().onActivityResult(this, requestCode, resultCode, data);
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
		that.tryPost(new File(filePath), message);
	}

	private void tryPost(File file, String message) {
		Session session = Session.getActiveSession();
		if (session.isOpened()) {
			if (session.getPermissions().contains(PERMISSION)) {
				uploadPhoto(session, file, message);
			} else {
				requestPermission(session, file, message);
			}
		} else {
			login(session, file, message);
		}
	}

	private void uploadPhoto(Session session, File file, String message) {
		try {
			final Request request = Request.newUploadPhotoRequest(session,
					file, new Request.Callback() {
						@Override
						public void onCompleted(Response response) {
							// toast?
						}
					});
			request.getParameters().putString("message", message);
			handler.post(new Runnable() {
				@Override
				public void run() {
					request.executeAsync();
				}
			});
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	private void requestPermission(Session session, final File file,
			final String message) {
		session.requestNewPublishPermissions(new NewPermissionsRequest(this,
				PERMISSION).setCallback(new StatusCallback() {
			@Override
			public void call(Session session, SessionState state,
					Exception exception) {
				if (state == SessionState.OPENED_TOKEN_UPDATED) {
					tryPost(file, message);
				}
			}
		}));
	}

	private void login(Session session, final File file, final String message) {
		StatusCallback callback = new StatusCallback() {
			@Override
			public void call(Session session, SessionState state, Exception exception) {
				if (state == SessionState.OPENED) {
					tryPost(file, message);
				}
			}
		};
        if (!session.isClosed()) {
            session.openForRead(new Session.OpenRequest(this).setCallback(callback));
        } else {
            Session.openActiveSession(this, true, callback);
        }
    }

}
