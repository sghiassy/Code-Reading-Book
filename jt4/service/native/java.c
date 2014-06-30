/* ========================================================================= *
 *                                                                           *
 *                 The Apache Software License,  Version 1.1                 *
 *                                                                           *
 *          Copyright (c) 1999-2001 The Apache Software Foundation.          *
 *                           All rights reserved.                            *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * Redistribution and use in source and binary forms,  with or without modi- *
 * fication, are permitted provided that the following conditions are met:   *
 *                                                                           *
 * 1. Redistributions of source code  must retain the above copyright notice *
 *    notice, this list of conditions and the following disclaimer.          *
 *                                                                           *
 * 2. Redistributions  in binary  form  must  reproduce the  above copyright *
 *    notice,  this list of conditions  and the following  disclaimer in the *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. The end-user documentation  included with the redistribution,  if any, *
 *    must include the following acknowlegement:                             *
 *                                                                           *
 *       "This product includes  software developed  by the Apache  Software *
 *        Foundation <http://www.apache.org/>."                              *
 *                                                                           *
 *    Alternately, this acknowlegement may appear in the software itself, if *
 *    and wherever such third-party acknowlegements normally appear.         *
 *                                                                           *
 * 4. The names  "The  Jakarta  Project",  "WebApp",  and  "Apache  Software *
 *    Foundation"  must not be used  to endorse or promote  products derived *
 *    from this  software without  prior  written  permission.  For  written *
 *    permission, please contact <apache@apache.org>.                        *
 *                                                                           *
 * 5. Products derived from this software may not be called "Apache" nor may *
 *    "Apache" appear in their names without prior written permission of the *
 *    Apache Software Foundation.                                            *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES *
 * INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY *
 * AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL *
 * THE APACHE  SOFTWARE  FOUNDATION OR  ITS CONTRIBUTORS  BE LIABLE  FOR ANY *
 * DIRECT,  INDIRECT,   INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR  CONSEQUENTIAL *
 * DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS *
 * OR SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) *
 * HOWEVER CAUSED AND  ON ANY  THEORY  OF  LIABILITY,  WHETHER IN  CONTRACT, *
 * STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN *
 * ANY  WAY  OUT OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF  ADVISED  OF THE *
 * POSSIBILITY OF SUCH DAMAGE.                                               *
 *                                                                           *
 * ========================================================================= *
 *                                                                           *
 * This software  consists of voluntary  contributions made  by many indivi- *
 * duals on behalf of the  Apache Software Foundation.  For more information *
 * on the Apache Software Foundation, please see <http://www.apache.org/>.   *
 *                                                                           *
 * ========================================================================= */

/* @version $Id: java.c,v 1.3 2001/08/06 21:54:33 jfclere Exp $ */
#include "jsvc.h"

#ifdef OS_CYGWIN
typedef long long __int64;
#endif
#include <jni.h>

static JavaVM *jvm=NULL;
static JNIEnv *env=NULL;
static jclass cls=NULL;

#define FALSE 0
#define TRUE !FALSE

static void shutdown(JNIEnv *env, jobject source, jboolean reload) {
    log_debug("Shutdown requested (reload is %d)",reload);
    if (reload==TRUE) main_reload();
    else main_shutdown();
}

char *java_library(arg_data *args, home_data *data) {
    char *libf=NULL;

    /* Did we find ANY virtual machine? */
    if (data->jnum==0) {
        log_error("Cannot find any VM in Java Home %s",data->path);
        return(false);
    }

    /* Select the VM */
    if (args->name==NULL) {
        libf=data->jvms[0]->libr;
        log_debug("Using default JVM in %s",libf);
    } else {
        int x;
        for (x=0; x<data->jnum; x++) {
            if (data->jvms[x]->name==NULL) continue;
            if (strcmp(args->name,data->jvms[x]->name)==0) {
                libf=data->jvms[x]->libr;
                log_debug("Using specific JVM in %s",libf);
                break;
            }
        }
        if (libf==NULL) {
            log_error("Invalid JVM name specified %s",args->name);
            return(NULL);
        }
    }
    return(libf);
}

/* Initialize the JVM and its environment, loading libraries and all */
bool java_init(arg_data *args, home_data *data) {
#ifdef OS_DARWIN
    dso_handle apph=NULL;
    char appf[1024];
#endif /* ifdef OS_DARWIN */
    jint (*symb)(JavaVM **, JNIEnv **, JavaVMInitArgs *);
    JNINativeMethod nativemethod;
    JavaVMOption *opt=NULL;
    dso_handle libh=NULL;
    JavaVMInitArgs arg;
    char *libf=NULL;
    jint ret;
    int x;

    /* Decide WHAT virtual machine we need to use */
    libf=java_library(args,data);
    if (libf==NULL) {
        log_error("Cannot locate JVM library file");
        return(false);
    }

    /* Initialize the DSO library */
    if (dso_init()!=true) {
        log_error("Cannot initialize the dynamic library loader");
        return(false);
    }

    /* Load the JVM library */
    libh=dso_link(libf);
    if (libh==NULL) {
        log_error("Cannot dynamically link to %s",libf);
        return(false);
    }
    log_debug("JVM library %s loaded",libf);

#ifdef OS_DARWIN
    /* MacOS/X actually has two libraries, one with the REAL vm, and one for
       the VM startup. The first one (libappshell.dyld) contains CreateVM */
    if (replace(appf,1024,"$JAVA_HOME/../Libraries/libappshell.dylib",
                 "$JAVA_HOME",data->path)!=0) {
        log_error("Cannot replace values in loader library");
        return(false);
    }
    apph=dso_link(appf);
    if (apph==NULL) {
        log_error("Cannot load required shell library %s",appf);
        return(false);
    }
    log_debug("Shell library %s loaded",appf);
#endif /* ifdef OS_DARWIN */
    symb=dso_symbol(libh,"JNI_CreateJavaVM");
    if (symb==NULL) {
#ifdef OS_DARWIN
        symb=dso_symbol(apph,"JNI_CreateJavaVM");
        if (symb==NULL) {
#endif /* ifdef OS_DARWIN */
            log_error("Cannot find JVM library entry point");
            return(false);
#ifdef OS_DARWIN
        }
#endif /* ifdef OS_DARWIN */
    }
    log_debug("JVM library entry point found (0x%08X)",symb);

    /* Prepare the VM initialization arguments */
    arg.version=JNI_VERSION_1_2;
    arg.ignoreUnrecognized=FALSE;
    arg.nOptions=args->onum;
    if (arg.nOptions==0) {
        arg.options=NULL;
    } else {
        opt=(JavaVMOption *)malloc(arg.nOptions*sizeof(JavaVMOption));
        for (x=0; x<args->onum; x++) {
            opt[x].optionString=args->opts[x];
            opt[x].extraInfo=NULL;
        }
        arg.options=opt;
    }

    /* Do some debugging */
    if (log_debug_flag==true) {
        log_debug("+-- DUMPING JAVA VM CREATION ARGUMENTS -----------------");
        log_debug("| Version:                       %x",arg.version);
        log_debug("| Ignore Unrecognized Arguments: %s",
                  arg.ignoreUnrecognized==TRUE?"True":"False");
        log_debug("| Extra options:                 %d",arg.nOptions);

        for (x=0; x<args->onum; x++) {
            log_debug("|   \"%s\" (0x%08x)",opt[x].optionString,
                      opt[x].extraInfo);
        }
        log_debug("+-------------------------------------------------------");
    }

    /* And finally create the Java VM */
    ret=(*symb)(&jvm, &env, &arg);
    if (ret!=0) {
        log_error("Cannot create Java VM");
        return(false);
    }
    log_debug("Java VM created successfully");

    cls=(*env)->FindClass(env,LOADER);
    if (cls==NULL) {
        log_error("Cannot find service loader %s",LOADER);
        return(false);
    }
    log_debug("Class %s found",LOADER);

    nativemethod.name="shutdown";
    nativemethod.signature="(Z)V";
    nativemethod.fnPtr=shutdown;
    if((*env)->RegisterNatives(env,cls,&nativemethod,1)!=0) {
        log_error("Cannot register native methods");
        return(false);
    }
    log_debug("Native methods registered");

    return(true);
}

/* Destroy the Java VM */
bool java_destroy(int exit) {
    jclass system=NULL;
    jmethodID method;

    system=(*env)->FindClass(env,"java/lang/System");
    if (system==NULL) {
        log_error("Cannot find class java/lang/System");
        return(false);
    }

    method=(*env)->GetStaticMethodID(env,system,"exit","(I)V");
    if (method==NULL) {
        log_error("Cannot found \"System.exit(int)\" entry point");
        return(false);
    }

    log_debug("Calling System.exit(%d)",exit);
    (*env)->CallStaticVoidMethod(env,cls,method,(jint)exit);

    /* We shouldn't get here, but just in case... */
    log_debug("Destroying the Java VM");
    if ((*jvm)->DestroyJavaVM(jvm)!=0) return(false);
    log_debug("Java VM destroyed");
    return(true);
}

/* Call the load method in our ServiceLoader class */
bool java_load(arg_data *args) {
    jclass stringClass=NULL;
    jstring className=NULL;
    jstring currentArgument=NULL;
    jobjectArray stringArray=NULL;
    jmethodID method=NULL;
    jboolean ret=FALSE;
    int x;

    className=(*env)->NewStringUTF(env,args->clas);
    if (className==NULL) {
        log_error("Cannot create string for class name");
        return(false);
    }

    stringClass=(*env)->FindClass(env,"java/lang/String");
    if (stringClass==NULL) {
        log_error("Cannot find class java/lang/String");
        return(false);
    }

    stringArray=(*env)->NewObjectArray(env,args->anum,stringClass,NULL);
    if (stringArray==NULL) {
        log_error("Cannot create arguments array");
        return(false);
    }

    for (x=0; x<args->anum; x++) {
        currentArgument=(*env)->NewStringUTF(env,args->args[x]);
        if (currentArgument==NULL) {
            log_error("Cannot create string for argument %s",args->args[x]);
            return(false);
        }
        (*env)->SetObjectArrayElement(env,stringArray,x,currentArgument);
    }

    method=(*env)->GetStaticMethodID(env,cls,"load",
                        "(Ljava/lang/String;[Ljava/lang/String;)Z");
    if (method==NULL) {
        log_error("Cannot found Service Loader \"load\" entry point");
        return(false);
    }

    ret=(*env)->CallStaticBooleanMethod(env,cls,method,className,stringArray);
    if (ret==FALSE) {
        log_error("Cannot load service");
        return(false);
    }

    log_debug("Service loaded successfully");
    return(true);
}

/* Call the start method in our service loader */
bool java_start(void) {
    jmethodID method;
    jboolean ret;

    method=(*env)->GetStaticMethodID(env,cls,"start","()Z");
    if (method==NULL) {
        log_error("Cannot found Service Loader \"start\" entry point");
        return(false);
    }

    ret=(*env)->CallStaticBooleanMethod(env,cls,method);
    if (ret==FALSE) {
        log_error("Cannot start service");
        return(false);
    }

    log_debug("Service started successfully");
    return(true);
}

/* Call the stop method in our service loader */
bool java_stop(void) {
    jmethodID method;
    jboolean ret;

    method=(*env)->GetStaticMethodID(env,cls,"stop","()Z");
    if (method==NULL) {
        log_error("Cannot found Service Loader \"stop\" entry point");
        return(false);
    }

    ret=(*env)->CallStaticBooleanMethod(env,cls,method);
    if (ret==FALSE) {
        log_error("Cannot stop service");
        return(false);
    }

    log_debug("Service stopped successfully");
    return(true);
}

/* Call the version method in our service loader */
bool java_version(void) {
    jmethodID method;

    method=(*env)->GetStaticMethodID(env,cls,"version","()V");
    if (method==NULL) {
        log_error("Cannot found Service Loader \"version\" entry point");
        return(false);
    }

    (*env)->CallStaticVoidMethod(env,cls,method);
    return(true);
}

/* Call the check method in our ServiceLoader class */
bool java_check(arg_data *args) {
    jstring className=NULL;
    jmethodID method=NULL;
    jboolean ret=FALSE;

    log_debug("Checking service");

    className=(*env)->NewStringUTF(env,args->clas);
    if (className==NULL) {
        log_error("Cannot create string for class name");
        return(false);
    }

    method=(*env)->GetStaticMethodID(env,cls,"check","(Ljava/lang/String;)Z");
    if (method==NULL) {
        log_error("Cannot found Service Loader \"check\" entry point");
        return(false);
    }

    ret=(*env)->CallStaticBooleanMethod(env,cls,method,className);
    if (ret==FALSE) {
        log_error("An error was detected checking the %s service",args->clas);
        return(false);
    }

    log_debug("Service checked successfully");
    return(true);
}

