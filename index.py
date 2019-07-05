import os
from bs4 import BeautifulSoup
import requests as rq
import time
import pymysql as pm
import lxml


def get_one_page(web_url):
    header = {'User-Agent': 'Chrome/64.0.3282.140 Safari/537.36 Edge/17.17134'}
    res = rq.get(web_url, headers=header)
    data = res.text
    if res.status_code == 200:
        try:
            with open("./maoyan.txt", 'a') as f1:
                f1.write(data)
        except IOError:
                os.abort()
        try:
            with open("./info.txt", 'a') as f2:
                soup = BeautifulSoup(data, 'html.parser')
                sp = soup.select('dl dd')
                for s in sp:
                    rank = s.find('i').text
                    name = s.find('p', class_='name').text
                    score = s.find('p', class_='score').text
                    star = s.find('p', class_='star').text.strip()
                    releasetime = s.find('p', class_='releasetime').text
                    conn = pm.connect(host = 'localhost', db = 'maoyan_movies', user = 'root', password = 'pi=3.14', charset='utf8')
                    cur = conn.cursor()

                    sql="create table if not exists movie(id varchar(20) not null,name varchar(100) not null, info varchar(120) not null, up_time varchar(120) not null)"
                    cur.execute(sql)

                    sql="insert into movie(id, name, info, up_time)" "values(%s,%s,%s,%s)"
                    param=(rank, name, star, releasetime)
                    cur.execute(sql, param)
                    conn.commit()
                    print("{}. {}\t{}\n{}\n{}\n\n".format(rank, name, score, star, releasetime));
                    try:
                        f2.write("{}. {}\t{}\n{}\n{}\n\n".format(rank, name, score, star, releasetime))
                    except IOError:
                        print("can't write!\n")
        except IOError:
            print("info.txt \n")
            os.abort()
    elif res.status_code/100 == 4:
        print(" ")
    elif res.status_code == 503:
        print(" ")
        for i in range(5):
            get_one_page(web_url)
            time.sleep(1)
        os.abort()
    elif res.status_code == 504:
        print("  ")
        for i in range(5):
            get_one_page(web_url)
            time.sleep(1)
        os.abort()


def main():
    for i in range(10):
        offset = i*10
        web_url = 'https://maoyan.com/board/4?offset=' + str(offset)
        get_one_page(web_url)
    print("\n")


if __name__ == '__main__':
    main()
