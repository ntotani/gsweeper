package org.cocos2dx.cpp;

import java.io.File;
import java.net.URL;
import java.sql.PreparedStatement;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.ContentValues;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.provider.MediaStore;
import android.provider.MediaStore.Images;
import android.util.Log;
import android.widget.Toast;

import com.facebook.FacebookAuthorizationException;
import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.FacebookRequestError;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.Session.OpenRequest;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.model.GraphObject;
import com.facebook.widget.FacebookDialog;

public class Cocos2dxActivity extends NativeActivity{

	private static Activity that = null;
	private static UiLifecycleHelper uiHelper;
	private static String TAG = "hoge";
	private static final List<String> PERMISSIONS = Arrays.asList("publish_actions");
	private Handler handler;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		that = this;
		uiHelper = new UiLifecycleHelper(this, null);
		handler = new Handler();
		
		// Facebook ログイン管理
        Session session = Session.getActiveSession();
        if (session == null) {
            if (savedInstanceState != null) {
                session = Session.restoreSession(this, null, statusCallback, savedInstanceState);
            }
            if (session == null) {
                session = new Session(this);
            }
            Session.setActiveSession(session);
            if (session.getState().equals(SessionState.CREATED_TOKEN_LOADED)) {
                //session.openForPublish(getOpenRequest());
                session.openForRead(new OpenRequest(this));
            }
        }
 
        // ログイン状態の確認
        if (! session.isOpened()) {
            doLogin();
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
	protected void onResume() {
		super.onResume();
		uiHelper.onResume();
	}
	
	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		uiHelper.onSaveInstanceState(outState);
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		uiHelper.onPause();
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		uiHelper.onDestroy();
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		uiHelper.onActivityResult(requestCode, resultCode, data,
				new FacebookDialog.Callback() {
					@Override
					public void onError(FacebookDialog.PendingCall pendingCall,
							Exception error, Bundle data) {
						Log.e("Activity",
								String.format("Error: %s", error.toString()));
					}

					@Override
					public void onComplete(
							FacebookDialog.PendingCall pendingCall, Bundle data) {
						Log.i("Activity", "Success!");
					}
				});
	}

	public static void tweet(String message, String filePath) {
		File file = new File(filePath);
		file.setReadable(true, false);
		// ContentValues values = new ContentValues();
		// values.put(Images.Media.MIME_TYPE, "image/jpeg");
		// values.put("_data", file.getAbsolutePath());
		// that.getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
		// values);
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
//		for (ApplicationInfo info : that.getPackageManager().getInstalledApplications(PackageManager.GET_META_DATA)) {
//			if (info.packageName.equals(packageName)) {
//				that.getPackageManager().qu
//				intent.setClassName(packageName, "com.twitter.applib.composer.TextFirstComposerActivity");
//				that.startActivity(intent);
//				return;
//			}
//		}
		that.startActivity(Intent.createChooser(intent, "共有"));

		/*
		if (FacebookDialog.canPresentShareDialog(that.getApplicationContext(),  
                FacebookDialog.ShareDialogFeature.SHARE_DIALOG)) {  
            try {  
                String name = "名前";  
                String url = "http://developer.android.com";  
                String description = "詳細";  
  File file = new File(filePath);
                // Fragment で発行するときは setFragment() を呼ぶ  
                FacebookDialog shareDialog = new FacebookDialog.ShareDialogBuilder(that).setApplicationName("appname").setCaption("caption").setDescription(description)  
                        .setName(name).setLink(url).setPicture(file.toURL().toString()).build();  
                uiHelper.trackPendingDialogCall(shareDialog.present());  
            } catch (Exception e) {  
                Toast.makeText(that, "Facebook でエラーが発生しました。", Toast.LENGTH_SHORT).show();  
            }  
        } 
        */
		//((Cocos2dxActivity)that).preparePublish();
	}
	
	private void doLogin() {
        Session session = Session.getActiveSession();
        Log.d(TAG,"doLogin: session state is " + session.getState() + ", isOpend:" + session.isOpened() + ", isClosed:" + session.isClosed());
        if (!session.isOpened()) {
            if (session.isClosed()) {
                session = new Session(this);
                Session.setActiveSession(session);          
            }
            session.openForRead(new OpenRequest(this));
        } else {
            Session.openActiveSession(this, true, statusCallback);
        }
    }
	
	private boolean hasPublishPermission() {
        Session session = Session.getActiveSession();
        return session != null && session.getPermissions().contains("publish_actions");
    }
 
    private void preparePublish() {
        Session session = Session.getActiveSession();
        if (session != null) {
            if (hasPublishPermission()) {
                // We can do the action right away.
                doPost();
            } else {
                // We need to get new permissions, then complete the action when we get called back.
                session.requestNewPublishPermissions(new Session.NewPermissionsRequest(this, PERMISSIONS));
            }
        }
    }
	
	private class SessionStatusCallback implements Session.StatusCallback {
        @Override
        public void call(Session session, SessionState state, Exception exception) {
            //updateView();
            Log.d(TAG, "SessionStatusCallback");
            onSessionStateChange(session, state, exception);
        }
    }
    private Session.StatusCallback statusCallback = new SessionStatusCallback();
    
    private void onSessionStateChange(Session session, SessionState state, Exception exception) {
        if ((exception instanceof FacebookOperationCanceledException ||
                exception instanceof FacebookAuthorizationException)) {
            Log.w(TAG, "error occured:" + exception.getMessage());
        } else if (state == SessionState.OPENED_TOKEN_UPDATED) {
            doPost();
        }
    }
    
    private void doPost() {
        Log.d(TAG,"doPost");
        final String message = "ほげほげ";
        final Request request = Request
                .newStatusUpdateRequest(Session.getActiveSession(), message, new Request.Callback() {
                    @Override
                    public void onCompleted(Response response) {
                        showPublishResult(message, response.getGraphObject(), response.getError());
                    }
                });
        handler.post(new Runnable() {
			@Override
			public void run() {
		        request.executeAsync();	
			}
		});
    }
    
    private interface GraphObjectWithId extends GraphObject {
        String getId();
    }
    
    private void showPublishResult(String message, GraphObject result, FacebookRequestError error) {
        Log.d(TAG,"showPublishResult");
        String title = null;
        String alertMessage = null;
        if (error == null) {
            title = "ステータスアップデート成功";
            String id = result.cast(GraphObjectWithId.class).getId();
            alertMessage = "ID=" + id;
        } else {
            title = "ステータスアップデート失敗";
            alertMessage = error.getErrorMessage();
        }
 
        new AlertDialog.Builder(this)
                .setTitle(title)
                .setMessage(alertMessage)
                .setPositiveButton("OK", null)
                .show();
    }
     

}
