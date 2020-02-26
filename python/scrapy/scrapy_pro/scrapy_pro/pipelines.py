# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://doc.scrapy.org/en/latest/topics/item-pipeline.html


class ScrapyProPipeline(object):

    def process_item(self, item, spider):
        title = item['title']
        link = item['link']
        content = item['content']
        with open('./result.txt', 'a+') as fw:
            fw.write("title:\n{}\n\nlink:\n{}\n\ncontent:\n{}\n\n".format(title, link, content))
            fw.write("\n\n----------------------------------------------------------------------\n\n")
        return item
