# -*- coding: utf-8 -*-
import scrapy
from bs4 import BeautifulSoup
from scrapy_pro.items import ScrapyProItem

class SantostangSpider(scrapy.Spider):
    name = 'santostang'
    allowed_domains = ['www.santostang.com']
    start_urls = ['http://www.santostang.com/']

    #def parse(self, response):
    #    #pass
    #    #print(response.text)
    #    #filename = "index.html"
    #    #with open(filename, 'w', encoding='utf-8') as fw:
    #    #    fw.write(response.text)

    #    soup = BeautifulSoup(response.text, "lxml")
    #    titles = soup.find_all('h1', class_='post-title')

    #    items = []
    #    for title in titles:
    #        #将信息封装到item,可以填充的字段有哪些在items.py中有定义
    #        item = ScrapyProItem()
    #        item['title'] = title.a.text.strip()
    #        item['link'] = title.a['href']
    #        items.append(item)
    #    return items

    def parse(self, response):
        soup = BeautifulSoup(response.text, "lxml")
        titles = soup.find_all('h1', class_='post-title')

        items = []
        for title in titles:
            #将信息封装到item,可以填充的字段有哪些在items.py中有定义
            item = ScrapyProItem()
            t = title.a.text.strip()
            l = title.a['href']
            item['title'] = t
            item['link'] = l
            #接着请求url,获得response后,调用callback,response.meta字典作为参数传给callback
            yield scrapy.Request(url=l, meta={'item':item}, callback=self.parse2)   

    def parse2(self, response):
        item = response.meta['item']    #接收传递的 item
        soup = BeautifulSoup(response.text, "lxml")
        c = soup.find('div', class_='view-content').text.strip().replace('\n',' ')

        item['content'] = c
        yield item

