#!/usr/bin/env python3

import smtplib
    
def sendmail(subject, content):
    import smtplib
    TO = 'julien_bourgeois@yahoo.fr'
    SUBJECT = subject;
    TEXT = content;
    
    # Gmail Sign In
    gmail_sender = 'hamster.bourgeois@gmail.com' 
    gmail_passwd = str('A(A)ca_RfjBMpfb+') 
    
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.ehlo()
    server.starttls()
    server.login(gmail_sender, gmail_passwd)
    
    BODY = '\r\n'.join(['To: %s' % TO,
                        'From: %s' % gmail_sender,
                        'Subject: %s' % SUBJECT,
                        '', TEXT])
    
    try:
        server.sendmail(gmail_sender, [TO], BODY)
        print ('email sent')
    except:
        print ('error sending mail')
        
    server.quit()

def main():
    sendmail("test subject","test content");
    
if __name__ == "__main__":
    main();
