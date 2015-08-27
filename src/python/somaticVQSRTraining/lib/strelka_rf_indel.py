#
# Starka
# Copyright (c) 2009-2014 Illumina, Inc.
#
# This software is provided under the terms and conditions of the
# Illumina Open Source Software License 1.
#
# You should have received a copy of the Illumina Open Source
# Software License 1 along with this program. If not, see
# <https://github.com/sequencing/licenses/>
#

# coding=utf-8

import numpy as np
import pandas
from vqsr import VQSRModel

import vqsr.tools.io as io

from sklearn.ensemble import RandomForestClassifier


class StrelkaRFIndel(VQSRModel):

    def __init__(self):
        self.clf = {}
        self.itypes = [1, 2]

    def train(self, tp, fp, columns, *args, **kwargs):
        """ Train model from sets of TPs and FPs

        :param tp: data frame with rows of TP instances.
        :type tp: pandas.DataFrame
        :param fp: data frame with rows of FP instances.
        :type fp: pandas.DataFrame
        :param columns: the feature columns to use
        """

        # import pdb; pdb.set_trace()

        tdf = pandas.DataFrame(tp[tp["NT"] == "ref"])
        fdf = pandas.DataFrame(fp[fp["NT"] == "ref"])

        tdf["tag"] = "TP"
        fdf["tag"] = "FP"

        allrows = pandas.concat([tdf, fdf])

        # TODO: parameters to try
        # {'max_depth' : range(1,20,1), 'n_estimators' : range(5, 30+1,5)},

        if not kwargs:
            kwargs = {"n_jobs": 8,
                      "max_depth": 4,
                      "n_estimators": 50,
                      "max_features": "log2"
                      }

        allrows["INDELTYPE"] = allrows["INDELTYPE"].astype(float).round().astype(int)

        self.clf = {}
        for it in self.itypes:
            self.clf[it] = RandomForestClassifier(**kwargs)
            irows = allrows[allrows["INDELTYPE"] == it]
            self.clf[it].fit(irows[columns].values, irows["tag"].values)


    def classify(self, instances, columns, *args, **kwargs):
        """ Classify a set of instances after training

        :param instances: data frame with instances.
        :type instances: pandas.DataFrame
        :param columns: the feature columns to use

        :return: data frame with added column "tag" which gives the classification
        :rtype: pandas.DataFrame
        """
        instances_nr = pandas.DataFrame(instances[instances["NT"] != "ref"])
        instances_nr["ptag"] = "FP"
        instances_nr["qual"] = 0
        insts = [instances_nr]

        instances = pandas.DataFrame(instances[instances["NT"] == "ref"])

        instances["INDELTYPE"] = instances["INDELTYPE"].astype(float).round().astype(int)

        for it in self.itypes:
            itinsts = pandas.DataFrame(instances[instances["INDELTYPE"] == it])
            preds = self.clf[it].predict(itinsts[columns].values)
            itinsts["ptag"] = preds
            cprobs = self.clf[it].predict_proba(itinsts[columns].values)
            tpcol = np.where(self.clf[it].classes_ == "TP")[0][0]
            itinsts["qual"] = cprobs[:, tpcol]
            insts.append(itinsts)

        return pandas.concat(insts)


    def save(self, filename):
        """ Save to file """
        if filename.endswith(".json"):
            io.write_classifier_json(self.clf, filename)
        else:
            io.write_classifier_pickle(self.clf, filename)

    def load(self, filename):
        """ Load from file """
        self.clf = io.read_pickled_classifier(filename)


    def plots(self, prefix, featurenames):
        """ Make diagnostic plots """

        for it in self.itypes:
            importances = self.clf[it].feature_importances_
            std = np.std([tree.feature_importances_ for tree in self.clf[it].estimators_],
                          axis=0)
            indices = np.argsort(importances)[::-1]

            # Print the feature ranking
            print "Feature ranking for INDELTYPE == %s:" % str(it)

            for f in xrange(0, len(indices)):
                print "%d. feature %d:%s (%f +- %f)" % (f + 1, indices[f],
                                                        featurenames[indices[f]],
                                                        importances[indices[f]],
                                                        std[indices[f]])

VQSRModel.register("strelka.rf.indel", StrelkaRFIndel)