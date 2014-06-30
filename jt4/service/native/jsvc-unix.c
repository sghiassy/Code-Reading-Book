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

/* @version $Id: jsvc-unix.c,v 1.3 2001/08/08 22:46:32 jfclere Exp $ */
#include "jsvc.h"

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>

extern char **environ;

pid_t controlled=0;
static bool stopping=false;
static bool doreload=false;
static void (*handler_int)(int)=NULL;
static void (*handler_hup)(int)=NULL;
static void (*handler_trm)(int)=NULL;

static void handler(int sig) {
    switch (sig) {
        case SIGTERM: {
            log_debug("Caught SIGTERM: Scheduling a shutdown");
            if (stopping==true) {
                log_error("Shutdown or reload already scheduled");
            } else {
                stopping=true;
            }
            if (handler_trm!=NULL) (*handler_trm)(sig);
            break;
        }

        case SIGINT: {
            log_debug("Caught SIGINT: Scheduling a shutdown");
            if (stopping==true) {
                log_error("Shutdown or reload already scheduled");
            } else {
                stopping=true;
            }
            if (handler_int!=NULL) (*handler_int)(sig);
            break;
        }

        case SIGHUP: {
            log_debug("Caught SIGHUP: Scheduling a reload");
            if (stopping==true) {
                log_error("Shutdown or reload already scheduled");
            } else {
                stopping=true;
                doreload=true;
            }
            if (handler_hup!=NULL) (*handler_hup)(sig);
            break;
        }

        default: {
            log_debug("Caught unknown signal %d",sig);
            break;
        }
    }
}

static bool checkuser(char *user, uid_t *uid, gid_t *gid) {
    struct passwd *pwds=NULL;
    int status=0;
    pid_t pid=0;

    /* Do we actually _have_ to switch user? */
    if (user==NULL) return(true);

    pwds=getpwnam(user);
    if (pwds==NULL) {
        log_error("Invalid user name '%s' specified",user);
        return(false);
    }

    *uid=pwds->pw_uid;
    *gid=pwds->pw_gid;

    /* Validate the user name in another process */
    pid=fork();
    if (pid==-1) {
        log_error("Cannot validate user name");
        return(false);
    }

    /* If we're in the child process, let's validate */
    if (pid==0) {
        if (setgid(*gid)!=0) {
            log_error("Cannot set group id for user '%s'",user);
            exit(1);
        }
        if (setuid(*uid)!=0) {
            log_error("Cannot set user id for user '%s'",user);
            exit(1);
        }
        /* If we got here we switched user/group */
        exit(0);
    }

    while (waitpid(pid,&status,0)!=pid);

    /* The child must have exited cleanly */
    if (WIFEXITED(status)) {
        status=WEXITSTATUS(status);

        /* If the child got out with 0 the user is ok */
        if (status==0) {
            log_debug("User '%s' validated",user);
            return(true);
        }
    }

    log_error("Error validating user '%s'",user);
    return(false);
}

#ifdef OS_CYGWIN
static void cygwincontroller() {
    raise(SIGTERM);
}
#endif
static void controller(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGINT:
        case SIGHUP:
            log_debug("Forwarding signal %d to process %d",sig,controlled);
            kill(controlled,sig);
            signal(sig,controller);
            break;
        default:
            log_debug("Caught unknown signal %d",sig);
            break;
    }
}

static int child(arg_data *args, home_data *data, uid_t uid, gid_t gid) {
    FILE *pidf=fopen(args->pidf,"w");
    pid_t pidn=getpid();
    int ret=0;

    if (pidf!=NULL) {
        fprintf(pidf,"%d\n",(int)pidn);
        fclose(pidf);
    } else {
        log_error("Cannot open PID file %s, PID is %d",args->pidf,pidn);
    }

    /* Initialize the Java VM */
    if (java_init(args,data)!=true) return(1);

    /* Check wether we need to dump the VM version */
    if (args->vers==true) {
        if (java_version()!=true) {
            return(-1);
        } else return(0);
    }

    /* Do we have to do a "check-only" initialization? */
    if (java_check(args)!=true) return(2);
    else if (args->chck==true) {
        printf("Service \"%s\" checked successfully\n",args->clas);
        return(0);
    }

    /* Load the service */
    if (java_load(args)!=true) return(3);

    /* Downgrade user */
    if (args->user!=NULL) {
        if (setgid(gid)!=0) {
            log_error("Cannot set group id for user '%s'",args->user);
            return(4);
        }
        if (setuid(uid)!=0) {
            log_error("Cannot set user id for user '%s'",args->user);
            return(4);
        }
    }

    /* Start the service */
    if (java_start()!=true) return(5);

    /* Install signal handlers */
    handler_hup=signal(SIGHUP,handler);
    handler_trm=signal(SIGTERM,handler);
    handler_trm=signal(SIGINT,handler);
    log_debug("Waiting for a signal to be delivered");
    while (!stopping) pause();
    log_debug("Shutdown or reload requested: exiting");

    /* Start the service */
    if (java_stop()!=true) return(6);

    if (doreload==true) ret=123;
    else ret=0;

    /* Destroy the Java VM */
    if (java_destroy(ret)!=true) return(7);

    return(ret);
}

int main(int argc, char *argv[]) {
    arg_data *args=NULL;
    home_data *data=NULL;
    int status=0;
    pid_t pid=0;
    uid_t uid=0;
    gid_t gid=0;

    /* Parse command line arguments */
    args=arguments(argc,argv);
    if (args==NULL) return(1);

    /* Let's check if we can switch user/group IDs */
    if (checkuser(args->user, &uid, &gid)==false) return(1);

    /* Retrieve JAVA_HOME layout */
    data=home(args->home);
    if (data==NULL) return(1);

    /* Check for help */
    if (args->help==true) {
        help(data);
        return(0);
    }

#ifdef OS_LINUX
    /* On some UNIX operating systems, we need to REPLACE this current
       process image with another one (thru execve) to allow the correct
       loading of VMs (notably this is for Linux). Set, replace, and go. */
    if (strcmp(argv[0],"jsvc.exec")!=0) {
        char *oldpath=getenv("LD_LIBRARY_PATH");
        char *libf=java_library(args,data);
        char *old=argv[0];
        char buf[2048];
        char *tmp=NULL;
        char *p1=NULL;
        char *p2=NULL;

        p1=strdup(libf);
        tmp=strrchr(p1,'/');
        if (tmp!=NULL) tmp[0]='\0';

        p2=strdup(p1);
        tmp=strrchr(p2,'/');
        if (tmp!=NULL) tmp[0]='\0';

        if (oldpath==NULL) snprintf(buf,2048,"%s:%s",p1,p2);
        else snprintf(buf,2048,"%s:%s:%s",oldpath,p1,p2);

        tmp=strdup(buf);
        setenv("LD_LIBRARY_PATH",tmp,1);

        log_debug("Invoking w/ LD_LIBRARY_PATH=%s",getenv("LD_LIBRARY_PATH"));

        argv[0]="jsvc.exec";
        execve(old,argv,environ);
        log_error("Cannot execute JSVC executor process");
        return(1);
    }
    log_debug("Running w/ LD_LIBRARY_PATH=%s",getenv("LD_LIBRARY_PATH"));
#endif /* ifdef OS_LINUX */

    /* If we have to detach, let's do it now */
    if (args->dtch==true) {
        pid=fork();
        if (pid==-1) {
            log_error("Cannot detach from parent process");
            return(1);
        }
        /* If we're in the parent process, we siply quit */
        if (pid!=0) return(0);
    }

    /* We have to fork: this process will become the controller and the other
       will be the child */
    while ((pid=fork())!=-1) {
        /* We forked (again), if this is the child, we go on normally */
        if (pid==0) exit(child(args,data,uid,gid));

        /* We are in the controller, we have to forward all interesting signals
           to the child, and wait for it to die */
        controlled=pid;
#ifdef OS_CYGWIN
       SetTerm(cygwincontroller);
#endif
        signal(SIGHUP,controller);
        signal(SIGTERM,controller);
        signal(SIGINT,controller);

        while (waitpid(pid,&status,0)!=pid);

        /* The child must have exited cleanly */
        if (WIFEXITED(status)) {
            status=WEXITSTATUS(status);

            /* If the child got out with 123 he wants to be restarted */
            if (status==123) {
                log_debug("Reloading service");
                continue;
            }
            /* If the child got out with 0 he is shutting down */
            if (status==0) {
                log_debug("Service shut down");
                return(0);
            }
            /* Otherwise we don't rerun it */
            log_error("Service exit with a return value of %d",status);
            return(1);
        } else {
            log_error("Service did not exit cleanly",status);
            return(1);
        }
    }

    /* Got out of the loop? A fork() failed then. */
    log_error("Cannot decouple controller/child processes");
    return(1);

}

void main_reload(void) {
    log_debug("Killing self with HUP signal");
    kill(getpid(),SIGHUP);
}

void main_shutdown(void) {
    log_debug("Killing self with TERM signal");
    kill(getpid(),SIGTERM);
}

