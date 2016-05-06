<?php

/**
 * @file
 * Contains \Drupal\redhen_relation\Form\RelationForm.
 */

namespace Drupal\redhen_relation\Form;

use Drupal\Core\Entity\ContentEntityForm;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form controller for Relation edit forms.
 *
 * @ingroup redhen_relation
 */
class RelationForm extends ContentEntityForm {
  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    /* @var $entity \Drupal\redhen_relation\Entity\Relation */
    $form = parent::buildForm($form, $form_state);

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $entity = $this->entity;
    $status = parent::save($form, $form_state);

    switch ($status) {
      case SAVED_NEW:
        drupal_set_message($this->t('Created %label.', [
          '%label' => $entity->label(),
        ]));
        break;

      default:
        drupal_set_message($this->t('Saved %label.', [
          '%label' => $entity->label(),
        ]));
    }
    $form_state->setRedirect('entity.redhen_relation.canonical', ['redhen_relation' => $entity->id()]);
  }

}
