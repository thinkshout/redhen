(function ($) {


  /**
   * Field instance settings screen: force the 'Display on registration form'
   * checkbox checked whenever 'Required' is checked.
   */
  Drupal.behaviors.redhenFieldsDisplayWidget = {
    attach: function (context, settings) {
      Drupal.behaviors.redhenFieldsDisplayWidget.hidden = 0;
      $('.redhen-email-widget-item').each(function(index, element) {
        if ($(element).find('.form-type-textfield input').attr('value') === '') {
          Drupal.behaviors.redhenFieldsDisplayWidget.hidden++;
          $(element).hide();
        }
        $(element).find('.remove').bind('click', function () {
          $(element).find('.form-text').attr('value', '');
          $(element).hide();
          $(element).appendTo( $('.field-widget-redhen-email-widget') );
          if ($(element).find('.form-radio').attr('checked') == true) {
            Drupal.redhenFieldsDefaultFirstItem();
          }
          Drupal.behaviors.redhenFieldsDisplayWidget.hidden++;
          if (Drupal.behaviors.redhenFieldsDisplayWidget.hidden > 0) {
            $('.field-widget-redhen-email-widget .add-another').show();
          }
          return false;
        });
      });
      
      $('.field-widget-redhen-email-widget .add-another').bind('click', function() {
        Drupal.redhenFieldsAddItem();
        return false;
      });
    }
  };

  Drupal.redhenFieldsAddItem = function () {
    $('.redhen-email-widget-item').each(function(index, element) {
      if ($(element).is(":visible") == false) {
        $(element).show();
        Drupal.behaviors.redhenFieldsDisplayWidget.hidden--;
        if (Drupal.behaviors.redhenFieldsDisplayWidget.hidden == 0) {
          $('.field-widget-redhen-email-widget .add-another').hide();
        }
        return false;
      }
    });
  };

  Drupal.redhenFieldsDefaultFirstItem = function () {
    $('.redhen-email-widget-item').each(function(index, element) {
      if ($(element).is(":visible") == true) {
        $(element).find('.form-radio').attr('checked', true);
      }
    });
  };


})(jQuery);
