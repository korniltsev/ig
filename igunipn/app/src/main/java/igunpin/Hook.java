package igunpin;

import android.util.Log;

import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.Proxy;
import java.net.URL;
import java.security.cert.Certificate;
import java.util.Arrays;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;

import javax.net.ssl.HttpsURLConnection;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XC_MethodHook;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

import static de.robv.android.xposed.XposedHelpers.findAndHookConstructor;
import static de.robv.android.xposed.XposedHelpers.findAndHookMethod;
import static de.robv.android.xposed.XposedHelpers.findClass;

public class Hook implements IXposedHookLoadPackage {

    public static final String TAG = "igunpin";
    private Certificate[] igCerts;

    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam p) throws Exception {
        if (p.packageName.equals("com.instagram.android")) {
            Log.d(TAG, "com.instagram.android loaded");
            getRealCerts();
            enableProxy(p);
            disableProxygen(p);
            unpinPlatform(p);
            unpingIG(p);
        }
    }

    private void enableProxy(XC_LoadPackage.LoadPackageParam p) {
        // replace openConnection(Proxy.DIRECT) with openConnection()
        final Class URL = findClass("java.net.URL", p.classLoader);
        final Class Proxy = findClass("java.net.Proxy", p.classLoader);
        findAndHookMethod(URL, "openConnection", Proxy, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                Method openConnection = URL.getDeclaredMethod("openConnection");
                param.setResult(openConnection.invoke(param.thisObject));
            }
        });
    }

    private void getRealCerts() throws Exception {
        // connect to https://i.instagram.com wi, and save certs
        FutureTask<Certificate[]> task = new FutureTask<>(new Callable<Certificate[]>() {
            @Override
            public Certificate[] call() throws Exception {
                Log.d(TAG, "get real certs from https://i.instagram.com");
                URL u = new URL("https://i.instagram.com");
                HttpURLConnection con = (HttpURLConnection) u.openConnection(Proxy.NO_PROXY);
                con.connect();
                Certificate[] serverCertificates = ((HttpsURLConnection) con).getServerCertificates();
                con.disconnect();
                Log.d(TAG, String.format("got real certs %s", Arrays.toString(serverCertificates)));
                return serverCertificates;
            }
        });
        new Thread(task).start();
        this.igCerts = task.get();

    }

    private void disableProxygen(XC_LoadPackage.LoadPackageParam p) {
        // disable
        try {
            Class HTTPClientBuilder = findClass("com.facebook.proxygen.HTTPClient$Builder", p.classLoader);
            findAndHookConstructor(HTTPClientBuilder, (XC_MethodHook) new FarewellLiger());
        } catch (XposedHelpers.ClassNotFoundError  e) {
            Class EventBase = findClass("com.facebook.proxygen.EventBase", p.classLoader);
            Class HTTPClient = findClass("com.facebook.proxygen.HTTPClient", p.classLoader);
            findAndHookConstructor(HTTPClient, EventBase, new FarewellLiger());
        }
    }

    private void unpinPlatform(XC_LoadPackage.LoadPackageParam p) {
        Class HttpsURLConnection = findClass("javax.net.ssl.HttpsURLConnection", p.classLoader);
        Class SSLSocketFactory = findClass("javax.net.ssl.SSLSocketFactory", p.classLoader);
        Class HostnameVerifier = findClass("javax.net.ssl.HostnameVerifier", p.classLoader);
        findAndHookMethod(HttpsURLConnection, "setSSLSocketFactory", SSLSocketFactory, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                param.setResult(null);
            }
        });
        findAndHookMethod(HttpsURLConnection, "setDefaultSSLSocketFactory", SSLSocketFactory, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                param.setResult(null);
            }
        });
        findAndHookMethod(HttpsURLConnection, "setHostnameVerifier", HostnameVerifier, new XC_MethodHook() {
            @Override
            protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
                param.setResult(null);
            }
        });
    }

    private void unpingIG(XC_LoadPackage.LoadPackageParam p) {
        Class HttpsURLConnection = findClass("com.android.okhttp.internal.huc.DelegatingHttpsURLConnection", p.classLoader);
        findAndHookMethod(HttpsURLConnection, "getServerCertificates", new XC_MethodHook() {
            @Override
            protected void afterHookedMethod(MethodHookParam param) throws Throwable {
                if (igCerts == null) {
                    Log.d(TAG, "replacing certs failed");
                } else {
                    URL url = ((HttpURLConnection) param.thisObject).getURL();
                    Log.d(TAG, String.format("replacing certs %s", url));
                    param.setResult(igCerts);
                }
            }
        });
    }

    private static class FarewellLiger extends XC_MethodHook {
        @Override
        protected void beforeHookedMethod(MethodHookParam param) throws Throwable {
            RuntimeException e = new RuntimeException("Farewell, liger! 🐈");
            Log.d(TAG, "Farewell, liger! 🐈");
            param.setThrowable(e);
        }
    }
}
