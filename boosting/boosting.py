from __future__ import annotations

from collections import defaultdict

import numpy as np
from sklearn.metrics import roc_auc_score
from sklearn.tree import DecisionTreeRegressor


def score(clf, x, y):
    return roc_auc_score(y == 1, clf.predict_proba(x)[:, 1])


class Boosting:

    def __init__(
            self,
            base_model_params: dict = None,
            n_estimators: int = 10,
            learning_rate: float = 0.1,
            subsample: float = 0.3,
            early_stopping_rounds: int = None,
            plot: bool = False,
    ):
        self.base_model_class = DecisionTreeRegressor
        self.base_model_params: dict = {} if base_model_params is None else base_model_params

        self.n_estimators: int = n_estimators

        self.models: list = []
        self.gammas: list = []

        self.learning_rate: float = learning_rate
        self.subsample: float = subsample

        self.early_stopping_rounds: int = early_stopping_rounds
        if early_stopping_rounds is not None:
            self.validation_loss = np.full(self.early_stopping_rounds, np.inf)

        self.plot: bool = plot

        self.history = defaultdict(list)

        self.sigmoid = lambda x: 1 / (1 + np.exp(-x))
        self.loss_fn = lambda y, z: -np.log(self.sigmoid(y * z)).mean()
        self.loss_derivative = lambda y, z: -y * self.sigmoid(-y * z)

    def fit_new_base_model(self, x, y, predictions):
        index = np.random.choice(np.arange(x.shape[0]), size=int(self.subsample * x.shape[0]))
        x_bootstrap, y_bootstrap = x[index], y[index]
        residuals = -self.loss_derivative(y_bootstrap, predictions[index])

        model = self.base_model_class(**self.base_model_params)
        model.fit(x_bootstrap, residuals)

        new_predictions = model.predict(x)
        gamma = self.find_optimal_gamma(y, predictions, new_predictions)
        
        self.gammas.append(gamma * self.learning_rate)
        self.models.append(model)

    def fit(self, x_train, y_train, x_valid, y_valid):
        """
        :param x_train: features array (train set)
        :param y_train: targets array (train set)
        :param x_valid: features array (validation set)
        :param y_valid: targets array (validation set)
        """
        train_predictions = np.zeros(y_train.shape[0])
        valid_predictions = np.zeros(y_valid.shape[0])
        
        best_valid_loss = np.inf
        stop_count = 0

        for estimator_index in range(self.n_estimators):
            self.fit_new_base_model(x_train, y_train, train_predictions)
            train_predictions += self.gammas[estimator_index] * self.models[estimator_index].predict(x_train)
            valid_predictions += self.gammas[estimator_index] * self.models[estimator_index].predict(x_valid)

            train_loss = self.loss_fn(y_train, train_predictions)
            valid_loss = self.loss_fn(y_valid, valid_predictions)
            self.history['train_loss'].append(train_loss)
            self.history['valid_loss'].append(valid_loss)

            if self.early_stopping_rounds is not None:
                if valid_loss < best_valid_loss:
                    best_valid_loss = valid_loss
                    stop_count = 0
                else:
                    stop_count += 1
                    if stop_count >= self.early_stopping_rounds:
                        break

        if self.plot:
            plt.figure(figsize=(15, 7))
            plt.plot(self.history['train_loss'], label='Train')
            plt.plot(self.history['valid_loss'], label='Valid')
            plt.xlabel('Iteration')
            plt.ylabel('Loss')
            plt.legend()
            plt.show()
            
    def predict_proba(self, x):
        total_predictions = np.zeros(x.shape[0])
        
        for gamma, model in zip(self.gammas, self.models):
             total_predictions += gamma * model.predict(x)
                
        class_1_probabilities = self.sigmoid(total_predictions)
        class_0_probabilities = 1 - class_1_probabilities

        return np.vstack((class_0_probabilities, class_1_probabilities)).T
    
    def find_optimal_gamma(self, y, old_predictions, new_predictions) -> float:
        gammas = np.linspace(start=0, stop=1, num=100)
        losses = [self.loss_fn(y, old_predictions + gamma * new_predictions) for gamma in gammas]

        return gammas[np.argmin(losses)]

    def score(self, x, y):
        return score(self, x, y)

    @property
    def feature_importances_(self):
        all_importances = np.sum([model.feature_importances_ for model in self.models], axis=0)
        normalized_importances = all_importances / all_importances.sum()
        return normalized_importances
