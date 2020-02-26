# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://doc.scrapy.org/en/latest/topics/items.html

import scrapy

# 定义item类型含有哪些字段,spiders/下面的爬虫会将解析到的字段
# 填充到这个结构体中,没有填充的就是空了
class ScrapyProItem(scrapy.Item):
    # define the fields for your item here like:
    #pass
    title = scrapy.Field()
    link = scrapy.Field()
    content = scrapy.Field()

