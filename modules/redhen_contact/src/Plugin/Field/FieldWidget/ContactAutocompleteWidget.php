<?php

namespace Drupal\redhen_contact\Plugin\Field\FieldWidget;

use Drupal\Core\Entity\Element\EntityAutocomplete;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Plugin implementation of the 'redhen_contact_autocomplete' widget.
 *
 * @FieldWidget(
 *   id = "redhen_contact_autocomplete",
 *   module = "redhen_contact",
 *   label = @Translation("Autocomplete for RedHen Contacts"),
 *   field_types = {
 *     "entity_reference"
 *   }
 * )
 */
class ContactAutocompleteWidget extends WidgetBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    return [
      'match_operator' => 'CONTAINS',
      'match_limit' => 10,
      'size' => 60,
      'placeholder' => '',
    ] + parent::defaultSettings();
  }

  /**
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
    $element['match_operator'] = [
      '#type' => 'radios',
      '#title' => $this->t('Autocomplete matching'),
      '#default_value' => $this->getSetting('match_operator'),
      '#options' => $this->getMatchOperatorOptions(),
      '#description' => $this->t('Select the method used to collect autocomplete suggestions. Note that <em>Contains</em> can cause performance issues on sites with thousands of entities.'),
    ];
    $element['match_limit'] = [
      '#type' => 'number',
      '#title' => $this->t('Number of results'),
      '#default_value' => $this->getSetting('match_limit'),
      '#min' => 1,
      '#description' => $this->t('The number of suggestions that will be listed. Min = 1, max = 100.'),
    ];
    $element['size'] = [
      '#type' => 'number',
      '#title' => $this->t('Size of textfield'),
      '#default_value' => $this->getSetting('size'),
      '#min' => 1,
      '#required' => TRUE,
    ];
    $element['placeholder'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Placeholder'),
      '#default_value' => $this->getSetting('placeholder'),
      '#description' => $this->t('Text that will be shown inside the field until a value is entered. This hint is usually a sample value or a brief description of the expected format.'),
    ];
    return $element;
  }

  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
    $operators = $this->getMatchOperatorOptions();

    $summary[] = $this->t('Autocomplete matching: @match_operator', ['@match_operator' => $operators[$this->getSetting('match_operator')]]);
    $summary[] = $this->t('Autocomplete suggestion list size: @size', ['@size' => $this->getSetting('match_limit')]);
    $summary[] = $this->t('Textfield size: @size', ['@size' => $this->getSetting('size')]);
    $summary[] = $this->t('Placeholder: @placeholder', ['@placeholder' => $this->getSetting('placeholder')]);

    return $summary;
  }

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    $contacts = $items->referencedEntities();
    $contact = isset($contacts[$delta]) ? $contacts[$delta] : NULL;
    $default_value = $contact ? EntityAutocomplete::getEntityLabels([$contact]) : '';

    $element += [
      '#type' => 'textfield',
      '#maxlength' => 1024,
      '#default_value' => $default_value,
      '#size' => $this->getSetting('size'),
      '#placeholder' => $this->getSetting('placeholder'),
      '#autocomplete_route_name' => 'redhen_contact.autocomplete',
      '#autocomplete_route_parameters' => [
        'match_operator' => $this->getSetting('match_operator'),
        'match_limit' => $this->getSetting('match_limit'),
      ],
      '#element_validate' => [
        [$this, 'validate'],
      ],
    ];

    return ['target_id' => $element];
  }

  /**
   * Validate the autocomplete field.
   */
  public function validate($element, FormStateInterface $form_state) {
    $element_value = $element['#value'];
    $target_id = EntityAutocomplete::extractEntityIdFromAutocompleteInput($element_value);

    if ($element_value && !$target_id) {
      $form_state->setError($element, $this->t('Invalid value.'));
    }

    $form_state->setValueForElement($element, $target_id);
  }

  /**
   * Returns the options for the match operator.
   *
   * @return array
   *   List of options.
   */
  protected function getMatchOperatorOptions() {
    return [
      'STARTS_WITH' => $this->t('Starts with'),
      'CONTAINS' => $this->t('Contains'),
    ];
  }

  /**
   * {@inheritDoc}
   */
  public static function isApplicable(FieldDefinitionInterface $field_definition) {
    $item_definition = $field_definition->getItemDefinition();
    if ($item_definition->getSetting('target_type') != 'redhen_contact') {
      return FALSE;
    }

    return parent::isApplicable($field_definition);
  }

}
